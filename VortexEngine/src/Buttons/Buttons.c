#include "Buttons.h"

#include "../Memory/Memory.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#else
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#endif

Button *g_pButton = NULL;

bool Buttons_init(uint8_t pin)
{
  g_pButton = (Button *)vmalloc(sizeof(Button));
  if (!g_pButton) {
    return false;
  }
  Button_init(g_pButton);
  return Button_initPin(g_pButton, pin);
}

void Buttons_cleanup(void)
{
  if (g_pButton) {
    Button_cleanup(g_pButton);
    vfree(g_pButton);
    g_pButton = NULL;
  }
}

void Buttons_update(void)
{
  Button_update(g_pButton);
#ifdef VORTEX_LIB
  Vortex_handleInputQueue(g_pButton, 1);
#endif
}
