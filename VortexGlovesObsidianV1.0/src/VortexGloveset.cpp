#include "VortexGloveset.h"

#include <Arduino.h>

#include "menus/Menu.h"

#include "TimeControl.h"
#include "LedControl.h"
#include "ColorTypes.h"
#include "Colorset.h"
#include "Settings.h"
#include "Buttons.h"
#include "Menus.h"
#include "Mode.h"

#include "Log.h"

bool VortexGloveset::init()
{
  // initialize a random seed
  // Always generate seed before creating button on digital pin 1(shared pin with analog 0)
  randomSeed(analogRead(0));

  if (!setupSerial()) {
    // error
    return false;
  }

  // initialize the time controller
  if (!Time::init()) {
    return false;
  }

  // initialize the settings
  if (!Settings::init()) {
    // error
    return false;
  }

  // setup led controller
  if (!Leds::init()) {
    // error
    return false;
  }

  // initialize the buttons
  if (!Buttons::init()) {
    // error
    return false;
  }

  // initialize the menus
  if (!Menus::init()) {
    // error
    return false;
  }

  return true;
}

void VortexGloveset::tick()
{
  // tick the current time counter forward
  Time::tickClock();

  // poll the button for changes
  g_pButton->check();

  // if the menus don't need to run, or they run and return false
  if (!Menus::shouldRun() || !Menus::run()) {
    // then just play the mode
    playMode();
  }

  //checkSerial();

  // update the leds
  Leds::update();
}

// ===================
//  private routines

bool VortexGloveset::setupSerial()
{
  // Setup serial communications
  Serial.begin(9600);
  // may want to add debug logic in here for attaching to a test framework
  return true;
}

// run the current mode
void VortexGloveset::playMode()
{
  // shortclick cycles to the next mode
  if (g_pButton->onShortClick()) {
    Settings::nextMode();
    Leds::clearAll();
  }

  // just in case there's no modes
  if (Settings::curMode()) {
    // play the current mode
    Settings::curMode()->play();
  }
}
