

#include "../c_types.h"
#ifdef VORTEX_LIB
#include "VortexLib.h"
#else
#endif

Button *g_pButton = NULL;
static Button s_button;

bool Buttons_init(uint8_t pin)
{
  g_pButton = &s_button;
  Button_init(g_pButton);
  return Button_initPin(g_pButton, pin);
}

void Buttons_cleanup(void)
{
  if (g_pButton) {
    Button_cleanup(g_pButton);
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
