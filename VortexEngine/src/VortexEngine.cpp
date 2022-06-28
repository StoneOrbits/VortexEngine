#include "VortexEngine.h"
#include "TimeControl.h"
#include "Infrared.h"
#include "Storage.h"
#include "Buttons.h"
#include "Serial.h"
#include "Modes.h"
#include "Menus.h"
#include "Leds.h"
#include "Log.h"

#include <Arduino.h>

bool VortexEngine::init()
{
  // initialize a random seed
  // Always generate seed before creating button on
  // digital pin 1 (shared pin with analog 0)
  randomSeed(analogRead(0));

  // all of the global controllers
  if (!SerialComs::init()) {
    DEBUG_LOG("Serial failed to initialize");
    return false;
  }
  if (!Time::init()) {
    DEBUG_LOG("Time failed to initialize");
    return false;
  }
  if (!Storage::init()) {
    DEBUG_LOG("Storage failed to initialize");
    return false;
  }
  if (!Infrared::init()) {
    DEBUG_LOG("Infrared failed to initialize");
    return false;
  }
  if (!Leds::init()) {
    DEBUG_LOG("Leds failed to initialize");
    return false;
  }
  if (!Buttons::init()) {
    DEBUG_LOG("Buttons failed to initialize");
    return false;
  }
  if (!Menus::init()) {
    DEBUG_LOG("Menus failed to initialize");
    return false;
  }
  if (!Modes::init()) {
    DEBUG_LOG("Settings failed to initialize");
    return false;
  }

  INFO_LOG("== Vortex Framework v" VORTEX_VERSION " (built " __TIMESTAMP__ ") ==");

  //Menus::openMenu(MENU_MODE_SHARING);

  return true;
}

void VortexEngine::cleanup()
{
  // cleanup in reverse order
  // NOTE: the arduino doesn't actually cleanup,
  //       but the test frameworks do
  Modes::cleanup();
  Menus::cleanup();
  Buttons::cleanup();
  Leds::cleanup();
  Infrared::cleanup();
  Storage::cleanup();
  Time::cleanup();
}

void VortexEngine::tick()
{
  // tick the current time counter forward
  Time::tickClock();

  // poll the buttons for changes
  Buttons::check();

  // check for serial communications
  SerialComs::checkSerial();

  // if the menus don't need to run, or they run and return false
  if (!Menus::run()) {
    // then just play the mode
    Modes::play();
  }

  // update the leds
  Leds::update();
}
