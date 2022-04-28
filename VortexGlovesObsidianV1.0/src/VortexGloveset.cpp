#include "VortexGloveset.h"

#include <Arduino.h>

#include "menus/Menu.h"

#include "TimeControl.h"
#include "ColorTypes.h"
#include "Colorset.h"
#include "Button.h"
#include "Mode.h"

#include "Log.h"

VortexGloveset::VortexGloveset() :
  m_button(),
  m_ringMenu(),
  m_pCurMenu(nullptr)
{
}

VortexGloveset::~VortexGloveset()
{
  // clean up global button pointer
  g_pButton = nullptr;
}

bool VortexGloveset::init()
{
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

  // initialize a random seed
  // Always generate seed before creating button on digital pin 1(shared pin with analog 0)
  randomSeed(analogRead(0));

  // initialize the button on pin 1
  if (!m_button.init(1)) {
    // error
    return false;
  }

  // setup the global button pointer
  g_pButton = &m_button;

  // initialize the ring menu and the menus it contains
  if (!m_ringMenu.init()) {
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
  m_button.check();

  // first try to run any menu logic
  if (!runAllMenus()) {
    // if it returns false then there was no menu logic to run
    // so just play the current mode
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

// run the menu logic, return false if menus are closed
bool VortexGloveset::runAllMenus()
{
  // if the user is holding the button down or the ringmenu is already open 
  // then run the ringmenu logic
  if (m_ringMenu.isOpen() || m_button.isPressed()) {
    return m_ringMenu.run();
  }

  // no menus to run
  return false;
}

// run the current mode
void VortexGloveset::playMode()
{
  // shortclick cycles to the next mode
  if (m_button.onShortClick()) {
    Settings::nextMode();
    Leds::clearAll();
  }

  if (!Settings::curMode()) {
    // no modes to play
    return;
  }

  // play the current mode
  Settings::curMode()->play();
}
