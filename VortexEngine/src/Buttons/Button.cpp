#include "Button.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

#ifdef VORTEX_EMBEDDED
#include "../VortexEngine.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// Update here to change button pin/port
#define PIN_NUM 2
#define PORT_LETTER C

// expands out details to make the macros work
#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define CONCATENATE_DETAIL_3(x, y, z) x##y##z
#define CONCATENATE_3(x, y, z) CONCATENATE_DETAIL_3(x, y, z)

// macros for the button pin/port
#define BUTTON_PORT   CONCATENATE(PORT, PORT_LETTER)
#define BUTTON_VPORT  CONCATENATE(VPORT, PORT_LETTER)
#define BUTTON_PIN    CONCATENATE_3(PIN, PIN_NUM, _bm)
#define PIN_CTRL      CONCATENATE_3(PIN, PIN_NUM, CTRL)
#define PORT_VECT     CONCATENATE_3(PORT, PORT_LETTER, _PORT_vect)

// interrupt handler to wakeup device on button press
ISR(PORT_VECT)
{
  // mark the interrupt as handled
  BUTTON_PORT.INTFLAGS = BUTTON_PIN;
  // turn off the interrupt
  BUTTON_PORT.PIN_CTRL &= ~PORT_ISC_gm;
  // call the wakeup routine in the engine
  VortexEngine_wakeup(true);
}

void Button_enableWake(Button *self)
{
  // turn on the above interrupt for FALLING edge, maintain the pullup enabled
  BUTTON_PORT.PIN_CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
}
#endif

void Button_make(Button *self)
{
  self->m_pressTime = 0;
  self->m_releaseTime = 0;
  self->m_holdDuration = 0;
  self->m_releaseDuration = 0;
  self->m_consecutivePresses = 0;
  self->m_releaseCount = 0;
  self->m_buttonState = false;
  self->m_newPress = false;
  self->m_newRelease = false;
  self->m_isPressed = false;
  self->m_shortClick = false;
  self->m_longClick = false;
}

void Button_destroy(Button *self)
{
}

bool Button_init(Button *self, uint8_t pin)
{
  self->m_pressTime = 0;
  self->m_releaseTime = 0;
  self->m_holdDuration = 0;
  self->m_releaseDuration = 0;
  self->m_consecutivePresses = 0;
  self->m_newPress = false;
  self->m_newRelease = false;
  self->m_shortClick = false;
  self->m_longClick = false;
  // this is weird, when I did m_releaseCount = !m_buttonState the
  // compiler generated a huge amount of assembly, but not !check()
  self->m_buttonState = Button_check(self);
  self->m_releaseCount = !Button_check(self);
  self->m_isPressed = self->m_buttonState;
#ifdef VORTEX_EMBEDDED
  BUTTON_PORT.PIN_CTRL = PORT_PULLUPEN_bm;
#endif
  return true;
}

bool Button_check(Button *self)
{
#ifdef VORTEX_LIB
  return (Vortex::vcallbacks()->checkPinHook(0) == 0);
#else
  return ((BUTTON_VPORT.IN & BUTTON_PIN) == 0);
#endif
}

void Button_update(Button *self)
{
  // reset the new press/release members this tick
  self->m_newPress = false;
  self->m_newRelease = false;

  // read the new button state
  bool newButtonState = Button_check(self);

  // did the button change (press/release occurred)
  if (newButtonState != self->m_buttonState) {
    // set the new state
    self->m_buttonState = newButtonState;
    // update the currently pressed member
    self->m_isPressed = self->m_buttonState;

    // update the press/release times and newpress/newrelease members
    if (self->m_isPressed) {
      // the button was just pressed
      self->m_pressTime = Time_getCurtime();
      self->m_newPress = true;
    } else {
      // simply ignore the first release, always. Because they just turned the device
      // on and we don't want them to cycle to the next mode or something
      if (self->m_releaseCount > 0) {
        // the button was just released
        self->m_releaseTime = Time_getCurtime();
        self->m_newRelease = true;
      }
      // count releases even inside the ignore window so that
      // we can tell if the button was released to stop ignoring
      self->m_releaseCount++;
    }
  }

  // calculate new hold/release durations if currently held/released
  if (self->m_isPressed) {
    // update the hold duration as long as the button is pressed
    if (Time_getCurtime() >= self->m_pressTime) {
      self->m_holdDuration = (uint32_t)(Time_getCurtime() - self->m_pressTime);
    }
  } else {
    // update the release duration as long as the button is released
    if (Time_getCurtime() >= self->m_releaseTime) {
      self->m_releaseDuration = (uint32_t)(Time_getCurtime() - self->m_releaseTime);
      if (self->m_releaseDuration > CONSECUTIVE_WINDOW_TICKS) {
        // if the release duration is greater than the threshold, reset the consecutive presses
        self->m_consecutivePresses = 0;
      }
    }
  }

  if (self->m_newRelease) {
    self->m_consecutivePresses++;
  }

  // whether a shortclick or long click just occurred
  self->m_shortClick = (self->m_newRelease && (self->m_holdDuration <= SHORT_CLICK_THRESHOLD_TICKS));
  self->m_longClick = (self->m_newRelease && (self->m_holdDuration > SHORT_CLICK_THRESHOLD_TICKS));

  if (self->m_shortClick) {
    DEBUG_LOG("Short click");
  }
  if (self->m_longClick) {
    DEBUG_LOG("Long click");
  }
}

bool Button_onConsecutivePresses(Button *self, uint8_t numPresses)
{
  if (self->m_consecutivePresses >= numPresses) {
    self->m_consecutivePresses = 0;
    return true;
  }
  return false;
}

bool Button_onPress(const Button *self) { return self->m_newPress; }
bool Button_onRelease(const Button *self) { return self->m_newRelease; }
bool Button_isPressed(const Button *self) { return self->m_isPressed; }
bool Button_onShortClick(const Button *self) { return self->m_shortClick; }
bool Button_onLongClick(const Button *self) { return self->m_longClick; }
uint32_t Button_pressTime(const Button *self) { return self->m_pressTime; }
uint32_t Button_releaseTime(const Button *self) { return self->m_releaseTime; }
uint32_t Button_holdDuration(const Button *self) { return self->m_holdDuration; }
uint32_t Button_releaseDuration(const Button *self) { return self->m_releaseDuration; }
uint8_t Button_consecutivePresses(const Button *self) { return self->m_consecutivePresses; }
uint8_t Button_releaseCount(const Button *self) { return self->m_releaseCount; }
