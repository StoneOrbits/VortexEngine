#include "Buttons.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

// Since there is only one button I am just going to expose a global pointer to
// access it, instead of making the Button class static in case a second button
// is added. This makes it easier to access the button from other places while
// still allowing for a second instance to be added.  I wish there was a more
// elegant way to make the button accessible but not global.
// This will simply point at Buttons::m_button.
Button *g_pButton = nullptr;

// static members
Button Buttons::m_buttons[NUM_BUTTONS];

bool Buttons::init()
{
  // initialize the button on pin 1
  if (!m_buttons[0].init(1)) {
    return false;
  }
  g_pButton = &m_buttons[0];
  return true;
}

void Buttons::cleanup()
{
}

void Buttons::check()
{
#ifndef VORTEX_LIB
  // on real devices ignore button presses at startup
  if (Time::getCurtime() <= IGNORE_BUTTON_TICKS) {
    return;
  }
#endif
  // would iterate all buttons and check them here
  // but there's only one button so
  for (uint32_t i = 0; i < NUM_BUTTONS; ++i) {
    m_buttons[i].check();
  }
#ifdef VORTEX_LIB
  // read input from the vortex lib interface, for example Vortex::shortClick()
  Vortex::handleInputQueue(m_buttons, NUM_BUTTONS);
#endif
}
