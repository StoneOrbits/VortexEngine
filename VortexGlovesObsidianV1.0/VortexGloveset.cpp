#include "VortexGloveset.h"

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#include <Arduino.h>

#include "Button.h"
#include "Time.h"

VortexGloveset::VortexGloveset() :
  m_button(),
  m_curMode(0),
  m_brightness(255),
  m_inMenu(false)
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
  updateLEDS();

  //Serial.println(m);
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

// run the menu logic, return false if nothing to do
bool VortexGloveset::runMenus()
{
  if (!m_inMenu || m_button.holdDuration() < 1000) {
    return false;
  }
  // must hold for 1000ms to enter the menu
  m_inMenu = true;

  // show menu

  return true;
}

// run the current mode
void VortexGloveset::playMode()
{
  // shortclick cycles to the next mode, wrapping at NUM_MODES
  if (m_button.onShortClick()) {
    m_curMode = (m_curMode + 1) % NUM_MODES;
  }

  // display modes[m_curMode]
}


void VortexGloveset::updateLEDs()
{
  FastLED.setBrightness(m_brightness);
  FastLED.show();
}
