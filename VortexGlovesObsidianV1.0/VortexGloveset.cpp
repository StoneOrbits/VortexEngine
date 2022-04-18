#include "VortexGloveset.h"

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#include <Arduino.h>

#include "GlobalBrightness.h"
#include "FactoryReset.h"
#include "ModeSharing.h"
#include "ColorSelect.h"
#include "PatternSelect"
#include "Randomizer.h"
#include "Button.h"
#include "Time.h"

VortexGloveset::VortexGloveset() :
  m_button(),
  m_curMode(0),
  m_inMenu(false),
  m_modeList()
{
  memset(m_leds, 0, sizeof(m_leds));
}

bool VortexGloveset::init()
{
  if (!setupSerial()) {
    // error
    return false;
  }

  if (!setupLEDs()) {
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
  if (!initMenus()) {
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
  turnOnPowerLED();

  return true;
}

void VortexGloveset::tick()
{
  // tick the current time counter forward
  tickClock();

  // poll the button for changes
  m_button.check();

  // run any menu logic
  if (!runMenus()) {
    // if the menus aren't running play the current mode
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

bool VortexGloveset::setupLEDs()
{
  // setup leds on data pin 4
  FastLED.addLeds<NEOPIXEL, 4>(m_leds, NUM_LEDS);
  FastLED.setBrightness(255);
  return true;
}

void VortexGloveset::turnOnPowerLED()
{
  Adafruit_DotStar strip = Adafruit_DotStar(1, 7, 8, DOTSTAR_BGR);
  strip.begin();
  strip.show();
}

void VortexGloveset::initMenus()
{
  // adding a menu consists of:
  //  name for sake of it
  //  menu object
  //  color
  
  // Create the randomizer and add it in first slot as white
  Randomizer *randomizer = new Randomizer(); 
  addMenu("randomizer", randomizer, HSV_WHITE);

  // Create the color select and add it second as orange
  ColorSelect *colSelect = new ColorSelect();
  addMenu("color select", colSelect, HSV_ORANGE);

  // Create the pattern select
  PatternSelect *patSelect = new PatternSelect();
  addMenu("pattern select", patSelect, HSV_BLUE);

  // create the global brightness menu
  GlobalBrightness *globBrightness = new GlobalBrightness();
  addMenu("global brightness", globBrightness, HSV_YELLOW);

  // createh factory reset menu
  FactoryReset *factReset = new FactoryReset();
  addMenu("factory reset", factReset, HSV_RED);

  // create the mode sharing menu
  ModeSharing *modeShare = new ModeSharing();
  addMenu("mode sharing", modeShare, HSV_TEAL);
}

void VortexGloveset::addMenu(const char *name, Menu *menu, HSVColor color)
{
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
  if (!m_pCurMenu || m_button.holdDuration() < 1000) {
    return false;
  }

  // run current menu if any is open
  if (m_pCurMenu) {
    // if the menu run handler returns false then exit menus
    if (!m_pCurMenu->run()) {
      // TODO save here?
      m_pCurMenu = NULL;
      return false;
    }
  }

  // show ring menu


  return true;
}

// run the current mode
void VortexGloveset::playMode()
{
  // shortclick cycles to the next mode, wrapping at number of modes
  if (m_button.onShortClick()) {
    m_curMode = (m_curMode + 1) % m_modeList.count();
  }

  // display modes[m_curMode]
}


void VortexGloveset::updateLEDs()
{
  FastLED.setBrightness(m_brightness);
  FastLED.show();
}
