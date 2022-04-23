#include "VortexGloveset.h"

#include <Arduino.h>

#include "menus/Menu.h"

#include "patterns/BasicPattern.h"

#include "TimeControl.h"
#include "ColorTypes.h"
#include "Colorset.h"
#include "Button.h"
#include "Mode.h"

VortexGloveset::VortexGloveset() :
  m_timeControl(),
  m_ledControl(),
  m_button(),
  m_curMode(0),
  m_modeList(),
  m_ringMenu(),
  m_pCurMenu(nullptr)
{
}

VortexGloveset::~VortexGloveset()
{
}

bool VortexGloveset::init()
{
  if (!setupSerial()) {
    // error
    return false;
  }

  // initialize the time controller
  if (!m_timeControl.init()) {
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

  // initialize the ring menu and the menus it contains
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
  
  return true;
}

void VortexGloveset::tick()
{
  // tick the current time counter forward
  m_timeControl.tickClock();

  // poll the button for changes
  m_button.check();

  // start by clearing each tick? So if nothing is done they clear at update
  m_ledControl.clearAll();

  // first try to run any menu logic
  if (!runAllMenus()) {
    // if it returns false then there was no menu logic to run
    // so just play the current mode
    playMode();
  }

  //checkSerial();
  
  // TODO: timestep?

  // update the leds
  m_ledControl.update();
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
  return false;
}

bool VortexGloveset::saveSettings() 
{
  return true;
}

void VortexGloveset::setDefaults() 
{
  // initialize the first mode
  BasicPattern *strobe = new BasicPattern(5, 8);
  Colorset *rgb = new Colorset(0xFF0000, 0xFF00, 0xFF);
  Mode *rgbStrobe = new Mode();
  rgbStrobe->bindAll(strobe, rgb);
  m_modeList.push_back(rgbStrobe);
}

// run the menu logic, return false if menus are closed
bool VortexGloveset::runAllMenus()
{
  // if there are no open menus and the ringmenu isn't open and the button 
  // isn't being pressed or the button is pressed but for less than 50 ms 
  // then there is nothing to do in the menu logic yet
  if (!m_pCurMenu && !m_ringMenu.isOpen() && 
      (!m_button.isPressed() || m_button.holdDuration() < 50)) {
    return false;
  }

  // if there is no menu open and the button is pressed
  if (!m_pCurMenu && m_button.isPressed()) {
    // run the ringmenu and assign any menu it returns
    // it is expected to return NULL most of the time
    m_pCurMenu = m_ringMenu.run();
    // if a menu was returned then init it with current mode
    if (m_pCurMenu) {
      // this allows the menu to operate on the current mode
      m_pCurMenu->init(m_modeList[m_curMode]);
    }
    return true;
  }

  // run current menu if any is open
  if (m_pCurMenu) {
    // run the click handlers for the current menu
    if (m_button.onShortClick()) { m_pCurMenu->onShortClick(); }
    if (m_button.onLongClick()) { m_pCurMenu->onLongClick(); }
    // if the menu run handler returns false then exit menus
    if (!m_pCurMenu->run()) {
      // TODO save here?
      // clear the current menu pointer
      m_pCurMenu = nullptr;
      // return false to let the mode play
      return false;
    }
    // nothing else after menu code has run
    return true;
  }

  // default return false to let the mode play
  return false;
}

// run the current mode
void VortexGloveset::playMode()
{
  if (!m_modeList.size()) {
    // no modes to play
    return;
  }

  // shortclick cycles to the next mode, wrapping at number of modes
  if (m_button.onShortClick()) {
    m_curMode = (m_curMode + 1) % m_modeList.size();
  }

  // play the current mode
  m_modeList[m_curMode]->play();
}
