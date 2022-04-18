#include "LedControl.h"

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#define NUM_LEDS      10
#define LED_DATA_PIN  4

#define POWER_LED_PIN 7
#define POWER_LED_CLK 8

LedControl::LedControl() :
  m_leds(),
  m_brightness(255),
{
}

bool LedControl::init()
{
  m_leds = vector<CRGB>(NUM_LEDS, 0);
  // setup leds on data pin 4
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(m_leds.data(), m_leds.size());
  FastLED.setBrightness(m_brightness);
  // create the power LED
  m_powerLED = Adafruit_DotStar(1, POWER_LED_PIN, POWER_LED_CLK, DOTSTAR_BGR);
  return true;
}

void LedControl::turnOnPowerLED()
{
  m_powerLED.begin();
  m_powerLED.show();
}

void LedControl::setIndex(int target, CHSV col)
{
  m_leds[target] = col;
}

void LedControl::setRange(int first, int last, CHSV col)
{
  for (int a = first; a <= last; a++) {
    setLed(a, col);
  }
}

void LedControl::clearAll(CHSV col)
{
  setRange(0, NUM_LEDS, col);
}

void LedControl::update()
{
  FastLED.show(m_brightness);
}
