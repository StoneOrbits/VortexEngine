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
  m_timeControl(),
  m_ledControl(),
  m_button(),
  m_settings(),
  m_ringMenu(),
  m_pCurMenu(nullptr),
  m_pCurMode(nullptr)
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

  // initialize the settings
  if (!m_settings.init()) {
    // error
    return false;
  }

  // get the current mode from settings
  m_pCurMode = m_settings.curMode();

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

  return true;
}

void VortexGloveset::tick()
{
  // tick the current time counter forward
  m_timeControl.tickClock();

  // poll the button for changes
  m_button.check();

  // start by clearing each tick? So if nothing is done they clear at update
  //m_ledControl.clearAll();

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

// run the menu logic, return false if menus are closed
bool VortexGloveset::runAllMenus()
{
  // if there is a menu open like Randomizer, Colorselect, etc
  if (m_pCurMenu) {
    // run just that menu
    return runCurMenu();
  }

  // if the user is holding the button down or the ringmenu is already open 
  // then run the ringmenu logic
  if (m_ringMenu.isOpen() || m_button.isPressed()) {
    return runRingMenu();
  }

  // no menus to run
  return false;
}

bool VortexGloveset::runCurMenu()
{
  // clear at start of menus
  g_pLedControl->clearAll();

  // first run the click handlers for the menu
  if (m_button.onShortClick()) { 
    m_pCurMenu->onShortClick(); 
  }
  if (m_button.onLongClick()) { 
    m_pCurMenu->onLongClick(); 
  }
  // if the menu run handler returns false that signals the 
  // menu was closed by the user leaving the menu
  if (!m_pCurMenu->run()) {
    // when a menu closes save all settings
    if (!m_settings.save()) {
      // error saving
    }
    // clear the current menu pointer
    m_pCurMenu = nullptr;
    // return false to let the mode play
    return false;
  }
  // continue in the opened menu
  return true;
}

bool VortexGloveset::runRingMenu()
{
  // clear at start of menus
  g_pLedControl->clearAll();
  // run the ringmenu and store any menu it returns,
  // it is expected to return NULL most of the time
  m_pCurMenu = m_ringMenu.run();
  // if no menu was returned then only continue if the ringmenu is open
  if (!m_pCurMenu) {
    return m_ringMenu.isOpen();
  }
  // otherwise initialiaze the new menu with the current mode
  if (!m_pCurMenu->init(m_pCurMode)) {
    // if the menu failed to init, don't open it
    m_pCurMenu = nullptr;
  }
  // contiue in the new selected menu
  return true;
}

// run the current mode
void VortexGloveset::playMode()
{
  // shortclick cycles to the next mode
  if (m_button.onShortClick()) {
    m_settings.nextMode();
    g_pLedControl->clearAll();
  }

  if (!m_pCurMode) {
    // no modes to play
    return;
  }

  // play the current mode
  m_settings.curMode()->play();
}
