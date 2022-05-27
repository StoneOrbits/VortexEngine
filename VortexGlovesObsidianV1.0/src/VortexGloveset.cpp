#include "VortexGloveset.h"
#include "TimeControl.h"
#include "LedControl.h"
#include "Storage.h"
#include "Buttons.h"
#include "Modes.h"
#include "Menus.h"
#include "Log.h"

#include <Arduino.h>

bool VortexGloveset::init()
{
  // initialize a random seed
  // Always generate seed before creating button on 
  // digital pin 1 (shared pin with analog 0)
  randomSeed(analogRead(0));
  // Setup serial communications
  Serial.begin(9600);

  // initialize the time controller
  if (!Time::init()) {
    DEBUG("Time failed to initialize");
    return false;
  }

  // setup led controller
  if (!Leds::init()) {
    DEBUG("Leds failed to initialize");
    return false;
  }

  // initialize the buttons
  if (!Buttons::init()) {
    DEBUG("Buttons failed to initialize");
    return false;
  }

  // initialize the menus
  if (!Menus::init()) {
    DEBUG("Menus failed to initialize");
    return false;
  }

  // initialize the storage
  if (!Storage::init()) {
    DEBUG("Storage failed to initialize");
    return false;
  }

  // initialize the modes
  if (!Modes::init()) {
    DEBUG("Settings failed to initialize");
    return false;
  }

  return true;
}

void VortexGloveset::tick()
{
  // tick the current time counter forward
  Time::tickClock();

  // poll the buttons for changes
  Buttons::check();

  // if the menus don't need to run, or they run and return false
  if (!Menus::run()) {
    // then just play the mode
    Modes::play();
  }

  //checkSerial();

  // update the leds
  Leds::update();
}
