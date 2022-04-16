#ifndef VORTEX_GLOVESET_H
#define VORTEX_GLOVESET_H

#include <FastLED.h>

#include "Button.h"

// the number of LEDs that are controlled by the board
#define NUM_LEDS    28

// number of individual modes
#define NUM_MODES   14

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
    CRGB m_leds[NUM_LEDS];

    // the current mode we're on
    uint32_t m_curMode;

    // global brightness
    uint32_t m_brightness;

    // whether inside the menus or at play mode
    bool m_inMenu;

    // ==================
    //  private routines

    // setup routines
    bool setupSerial();
    bool setupLEDs();
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
