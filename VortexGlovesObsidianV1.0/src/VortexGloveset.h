#ifndef VORTEX_GLOVESET_H
#define VORTEX_GLOVESET_H

#include <FastLED.h>

#include "TimeControl.h"
#include "LedControl.h"
#include "ColorTypes.h"
#include "RingMenu.h"
#include "Settings.h"
#include "Button.h"

class Menu;
class Mode;

class VortexGloveset
{
public:
  VortexGloveset();
  ~VortexGloveset();

  // setup and initialization function
  bool init();

  // tick function for each loop
  void tick();

private:
  // ==================
  //  private routines

  // setup routines
  bool setupSerial();
  // run the menu logic, return false if nothing to do
  bool runAllMenus();
  // run the current mode
  void playMode();

  // ==============
  //  private data

  // the button on the gloveset (g_pButton will point at this)
  Button m_button;

  // the current menu that is open (if any)
  Menu *m_pCurMenu;

#ifdef TEST_FRAMEWORK
  // so the test framework can access stuff
  friend class TestFramework;
#endif
};

#endif
