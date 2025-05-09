#include "Buttons.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#else
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#endif

// Since there is only one button I am just going to expose a global pointer to
// access it, instead of making the Button class static in case a second button
// is added. This makes it easier to access the button from other places while
// still allowing for a second instance to be added.  I wish there was a more
// elegant way to make the button accessible but not global.
// This will simply point at Buttons::m_button.

// Button Left
Button *g_pButtonL = nullptr;
// Button Mid
Button *g_pButtonM = nullptr;
// Button Right
Button *g_pButtonR = nullptr;

// static members
Button Buttons::m_buttons[NUM_BUTTONS];
// for cancelling the VL Sender
uint8_t Buttons::m_cancelInterruptPin = 0;
volatile bool Buttons::cancelButtonPressed = false;

bool Buttons::init()
{
  // initialize the button on pins 5/6/7
  if (!m_buttons[0].init(BUTTON_L_PIN) ||
      !m_buttons[1].init(BUTTON_M_PIN) ||
      !m_buttons[2].init(BUTTON_R_PIN)) {
    return false;
  }
  g_pButtonL = &m_buttons[0];
  g_pButtonM = &m_buttons[1];
  g_pButtonR = &m_buttons[2];
  return true;
}

void Buttons::cleanup()
{
}

void Buttons::update()
{
  // would iterate all buttons and check them here
  // but there's only one button so
  for (uint8_t i = 0; i < NUM_BUTTONS; ++i) {
    m_buttons[i].update();
  }
#ifdef VORTEX_LIB
  // read input from the vortex lib interface, for example Vortex::shortClick()
  Vortex::handleInputQueue(m_buttons, NUM_BUTTONS);
#endif
}

#ifdef VORTEX_EMBEDDED
void IRAM_ATTR Buttons::cancelButtonISR() {
  cancelButtonPressed = true;
}
#endif

void Buttons::beginCancelInterrupt(uint8_t pin) {
  cancelButtonPressed = false;
  m_cancelInterruptPin = pin;
#ifdef VORTEX_EMBEDDED
  attachInterrupt(digitalPinToInterrupt(pin), cancelButtonISR, RISING);
#endif
}

void Buttons::endCancelInterrupt() {
#ifdef VORTEX_EMBEDDED
  detachInterrupt(digitalPinToInterrupt(m_cancelInterruptPin));
#endif
}

bool Buttons::isCancelRequested() {
  return cancelButtonPressed;
}

