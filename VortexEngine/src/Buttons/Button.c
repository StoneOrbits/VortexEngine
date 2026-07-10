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

#define PIN_NUM 2
#define PORT_LETTER C

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define CONCATENATE_DETAIL_3(x, y, z) x##y##z
#define CONCATENATE_3(x, y, z) CONCATENATE_DETAIL_3(x, y, z)

#define BUTTON_PORT   CONCATENATE(PORT, PORT_LETTER)
#define BUTTON_VPORT  CONCATENATE(VPORT, PORT_LETTER)
#define BUTTON_PIN    CONCATENATE_3(PIN, PIN_NUM, _bm)
#define PIN_CTRL      CONCATENATE_3(PIN, PIN_NUM, CTRL)
#define PORT_VECT     CONCATENATE_3(PORT, PORT_LETTER, _PORT_vect)

ISR(PORT_VECT)
{
  BUTTON_PORT.INTFLAGS = BUTTON_PIN;
  BUTTON_PORT.PIN_CTRL &= ~PORT_ISC_gm;
  VortexEngine_wakeup(true);
}

void Button_enableWake(Button *self)
{
  BUTTON_PORT.PIN_CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
}
#endif

void Button_init(Button *self)
{
  self->pressTime = 0;
  self->releaseTime = 0;
  self->holdDuration = 0;
  self->releaseDuration = 0;
  self->consecutivePresses = 0;
  self->releaseCount = 0;
  self->buttonState = false;
  self->newPress = false;
  self->newRelease = false;
  self->isPressed = false;
  self->shortClick = false;
  self->longClick = false;
}

void Button_cleanup(Button *self)
{
}

bool Button_initPin(Button *self, uint8_t pin)
{
  self->pressTime = 0;
  self->releaseTime = 0;
  self->holdDuration = 0;
  self->releaseDuration = 0;
  self->consecutivePresses = 0;
  self->newPress = false;
  self->newRelease = false;
  self->shortClick = false;
  self->longClick = false;
  self->buttonState = Button_check(self);
  self->releaseCount = !Button_check(self);
  self->isPressed = self->buttonState;
#ifdef VORTEX_EMBEDDED
  BUTTON_PORT.PIN_CTRL = PORT_PULLUPEN_bm;
#endif
  return true;
}

bool Button_check(Button *self)
{
#ifdef VORTEX_LIB
  return (Vortex_vcallbacks()->checkPinHook(0) == 0);
#else
  return ((BUTTON_VPORT.IN & BUTTON_PIN) == 0);
#endif
}

void Button_update(Button *self)
{
  self->newPress = false;
  self->newRelease = false;

  bool newButtonState = Button_check(self);

  if (newButtonState != self->buttonState) {
    self->buttonState = newButtonState;
    self->isPressed = self->buttonState;

    if (self->isPressed) {
      self->pressTime = Time_getCurtime();
      self->newPress = true;
    } else {
      if (self->releaseCount > 0) {
        self->releaseTime = Time_getCurtime();
        self->newRelease = true;
      }
      self->releaseCount++;
    }
  }

  if (self->isPressed) {
    if (Time_getCurtime() >= self->pressTime) {
      self->holdDuration = (uint32_t)(Time_getCurtime() - self->pressTime);
    }
  } else {
    if (Time_getCurtime() >= self->releaseTime) {
      self->releaseDuration = (uint32_t)(Time_getCurtime() - self->releaseTime);
      if (self->releaseDuration > CONSECUTIVE_WINDOW_TICKS) {
        self->consecutivePresses = 0;
      }
    }
  }

  if (self->newRelease) {
    self->consecutivePresses++;
  }

  self->shortClick = (self->newRelease && (self->holdDuration <= SHORT_CLICK_THRESHOLD_TICKS));
  self->longClick = (self->newRelease && (self->holdDuration > SHORT_CLICK_THRESHOLD_TICKS));

  if (self->shortClick) {
    DEBUG_LOG("Short click");
  }
  if (self->longClick) {
    DEBUG_LOG("Long click");
  }
}

bool Button_onPress(const Button *self)
{
  return self->newPress;
}

bool Button_onRelease(const Button *self)
{
  return self->newRelease;
}

bool Button_isPressed(const Button *self)
{
  return self->isPressed;
}

bool Button_onShortClick(const Button *self)
{
  return self->shortClick;
}

bool Button_onLongClick(const Button *self)
{
  return self->longClick;
}

bool Button_onConsecutivePresses(Button *self, uint8_t numPresses)
{
  if (self->consecutivePresses >= numPresses) {
    self->consecutivePresses = 0;
    return true;
  }
  return false;
}

uint32_t Button_pressTime(const Button *self)
{
  return self->pressTime;
}

uint32_t Button_releaseTime(const Button *self)
{
  return self->releaseTime;
}

uint32_t Button_holdDuration(const Button *self)
{
  return self->holdDuration;
}

uint32_t Button_releaseDuration(const Button *self)
{
  return self->releaseDuration;
}

uint8_t Button_consecutivePresses(const Button *self)
{
  return self->consecutivePresses;
}

uint8_t Button_releaseCount(const Button *self)
{
  return self->releaseCount;
}
