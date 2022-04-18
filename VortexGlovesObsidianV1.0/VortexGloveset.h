#ifndef VORTEX_GLOVESET_H
#define VORTEX_GLOVESET_H

#include <FastLED.h>

#include <vector>

#include "Button.h"
#include "Mode.h"

// the number of LEDs that are controlled by the board
#define NUM_LEDS    10

class VortexGloveset
{
  private:
    // private unimplemented copy and assignment constructors to prevent copies
    VortexGloveset(VortexGloveset const&);
    void operator=(VortexGloveset const&);

  public:
    // constructor
    VortexGloveset();

    // setup and initialization function
    bool init();

    // tick function for each loop
    void tick();

  private:
    // ==============
    //  private data

    // the button on the gloveset
    Button m_button;

    // array of led color values
    vector<CRGB> m_leds;

    // the current mode we're on
    uint32_t m_curMode;

    // global brightness
    uint32_t m_brightness;

    // current menu that is open
    Menu m_pCurMenu;

    // list of all modes in the gloveset
    vector<Mode *> m_modeList;

    // ==================
    //  private routines

    // setup routines
    bool setupSerial();
    bool setupLEDs();
    void initMenus();
    void turnOnPowerLED();

    // settings save/load
    bool loadSettings();
    bool saveSettings();
    
    // set default settings (must save after)
    void setDefaults();

    // run the menu logic, return false if nothing to do
    bool runMenus();

    // run the current mode
    void playMode();

    // update and render the LEDs
    void updateLEDs();
};

#endif
