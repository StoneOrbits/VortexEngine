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
Button *g_pButton = nullptr;

bool Buttons::init()
{
  // initialize the button on pin 9
  g_pButton = new Button();
  if (!g_pButton) {
    return false;
  }
  return true;
}

void Buttons::cleanup()
{
  delete g_pButton;
  g_pButton = nullptr;
}

void Buttons::update()
{
  // would iterate all buttons and check them here
  // but there's only one button so
  g_pButton->update();
#ifdef VORTEX_LIB
  // read input from the vortex lib interface, for example Vortex::shortClick()
  Vortex::handleInputQueue(g_pButton, NUM_BUTTONS);
#endif
}
