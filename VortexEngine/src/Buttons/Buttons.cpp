#include "Buttons.h"

#include "../Memory/Memory.h"

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
// This will simply point at the static button below.
Button *g_pButton = NULL;

// the storage for the button(s), these are static so they never touch the heap
static Button s_buttons[NUM_BUTTONS];

bool Buttons_init()
{
  g_pButton = &s_buttons[0];
  Button_make(g_pButton);
  // init the button on pin 1
  return Button_init(g_pButton, 1);
}

void Buttons_cleanup()
{
  if (g_pButton) {
    Button_destroy(g_pButton);
    g_pButton = NULL;
  }
}

uint8_t Buttons_numButtons()
{
  return NUM_BUTTONS;
}

void Buttons_update()
{
  Button_update(g_pButton);
#ifdef VORTEX_LIB
  // read input from the vortex lib interface, for example Vortex::shortClick()
  Vortex::handleInputQueue(g_pButton, NUM_BUTTONS);
#endif
}
