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
  private:
    // private unimplemented copy and assignment constructors to prevent copies
    VortexGloveset(VortexGloveset const&);
    void operator=(VortexGloveset const&);

  public:
    VortexGloveset();
    ~VortexGloveset();

    // setup and initialization function
    bool init();

    // tick function for each loop
    void tick();

    // add a mode to the mode list
    bool addMode(Mode *mode);
    // remove a mode from an index, shifting above modes down one
    bool removeMode(uint32_t index);

  private:
    // ==============
    //  private data

    // time manager and controller
    TimeControl m_timeControl;

    // the LED controller
    LedControl m_ledControl;

    // the button on the gloveset
    Button m_button;

    // the settings of the gloveset
    Settings m_settings;

    // the ring menu
    RingMenu m_ringMenu;

    // the current menu that is open (if any)
    Menu *m_pCurMenu;

    // the current mode that is selected (if any)
    Mode *m_pCurMode;

    // ==================
    //  private routines

    // setup routines
    bool setupSerial();

    // run the menu logic, return false if nothing to do
    bool runAllMenus();

    // runs menu logic for the current open menu
    bool runCurMenu();

    // runs logic for the ring menu
    bool runRingMenu();

    // run the current mode
    void playMode();
};

#endif
