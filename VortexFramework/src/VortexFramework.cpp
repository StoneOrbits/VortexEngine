#include "VortexFramework.h"
#include "TimeControl.h"
#include "Infrared.h"
#include "Storage.h"
#include "Buttons.h"
#include "Modes.h"
#include "Menus.h"
#include "Leds.h"
#include "Log.h"

#include <Arduino.h>

bool VortexFramework::init()
{
  // initialize a random seed
  // Always generate seed before creating button on 
  // digital pin 1 (shared pin with analog 0)
  randomSeed(analogRead(0));
  // Setup serial communications
  Serial.begin(9600);

  // all of the global controllers
  if (!Time::init()) {
    DEBUG("Time failed to initialize");
    return false;
  }
  if (!Storage::init()) {
    DEBUG("Storage failed to initialize");
    return false;
  }
  if (!Infrared::init()) {
    DEBUG("Infrared failed to initialize");
    return false;
  }
  if (!Leds::init()) {
    DEBUG("Leds failed to initialize");
    return false;
  }
  if (!Buttons::init()) {
    DEBUG("Buttons failed to initialize");
    return false;
  }
  if (!Menus::init()) {
    DEBUG("Menus failed to initialize");
    return false;
  }
  if (!Modes::init()) {
    DEBUG("Settings failed to initialize");
    return false;
  }

  return true;
}

void VortexFramework::cleanup()
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

void VortexFramework::tick()
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
