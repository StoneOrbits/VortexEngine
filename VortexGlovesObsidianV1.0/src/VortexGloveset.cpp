#include "VortexGloveset.h"
#include "TimeControl.h"
#include "LedControl.h"
#include "Buttons.h"
#include "Modes.h"
#include "Menus.h"
#include "Log.h"

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

  // initialize the modes
  if (!Modes::init()) {
    DEBUG("Settings failed to initialize");
    return false;
  }

  return true;
}

void VortexGloveset::tick()
{
  uint32_t start = micros();
  DEBUGF("Tick start: %u", start);

  // tick the current time counter forward
  Time::tickClock();

  uint32_t log_start = micros();
  DEBUGF("Tick logic start: %u (+%uus)", log_start, log_start - start);

  // poll the buttons for changes
  Buttons::check();

  // if the menus don't need to run, or they run and return false
  if (!Menus::run()) {
    // then just play the mode
    Modes::play();
  }

  uint32_t log_end = micros();
  DEBUGF("Tick logic end: %u (+%uus)", log_end, log_end - log_start);

  //checkSerial();

  // update the leds
  Leds::update();

  uint32_t end = micros();
  DEBUGF("Tick end: %u (+%uus)", end, end - log_end);
}
