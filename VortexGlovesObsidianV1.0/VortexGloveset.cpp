#include "VortexGloveset.h"

#include <Arduino.h>

#include "Button.h"
#include "Color.h"
#include "Time.h"

using namespace std;

VortexGloveset::VortexGloveset() :
  m_button(),
  m_ledControl(),
  m_curMode(0),
  m_modeList(),
  m_ringMenu(),
  m_pCurMenu(nullptr)
{
}

bool VortexGloveset::init()
{
  if (!setupSerial()) {
    // error
    return false;
  }

  // setup led controller
  if (!m_ledControl.init()) {
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

  // initialize all the menus
  if (!m_ringMenu.init()) {
    // error
    return false;
  }

  // try to load settings
  if (!loadSettings()) {
    // if couldn't load any settings use defaults
    setDefaults();
    // save the default settings
    saveSettings();
  }
  
  // the device is now turned on
  m_ledControl.turnOnPowerLED();

  return true;
}

void VortexGloveset::tick()
{
  // tick the current time counter forward
  tickClock();

  // poll the button for changes
  m_button.check();

  // first try to run any menu logic
  if (!runMenus()) {
    // if it returns false then there was no menu logic to run
    // so just play the current mode
    playMode();
  }

  //checkSerial();
  
  // TODO: timestep?

  // update the leds
  updateLEDs();
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

bool VortexGloveset::loadSettings() 
{
  return true;
}

bool VortexGloveset::saveSettings() 
{
  return true;
}

void VortexGloveset::setDefaults() 
{
}

// run the menu logic, return false if menus are closed
bool VortexGloveset::runMenus()
{
  // if the ringmenu isn't open or the button isn't being pressed,
  // or the button is pressed but for less than 50 ms then 
  // there is nothing to do in the menu logic yet
  if (!m_ringMenu.isOpen() && 
      (!m_button.isPressed() || m_button.holdDuration() < 50)) {
    return false;
  }

  // run the ringmenu and it will return a pointer to the current menu
  if (!m_pCurMenu && m_button.isPressed()) {
    m_pCurMenu = m_ringMenu.run();
    return true;
  }

  // run current menu if any is open
  if (m_pCurMenu) {
    // if the menu run handler returns false then exit menus
    if (!m_pCurMenu->run()) {
      // TODO save here?
      m_pCurMenu = NULL;
      return NULL;
    }
    // nothing else after menu code has run
    return NULL;
  }

  return true;
}

// run the current mode
void VortexGloveset::playMode()
{
  // shortclick cycles to the next mode, wrapping at number of modes
  if (m_button.onShortClick()) {
    m_curMode = (m_curMode + 1) % m_modeList.size();
  }

  // display modes[m_curMode]
}

