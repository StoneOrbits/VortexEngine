#include "LedControl.h"

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#define LED_DATA_PIN  4

#define POWER_LED_PIN 7
#define POWER_LED_CLK 8

using namespace std;

LedControl::LedControl() :
  m_leds(),
  m_brightness(255),
  m_onboardLED(1, POWER_LED_PIN, POWER_LED_CLK, DOTSTAR_BGR)
{
}

bool LedControl::init()
{
  // create the array of LEDS
  m_leds = vector<CRGB>(NUM_LEDS, 0);
  // setup leds on data pin 4
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(m_leds.data(), m_leds.size());
  FastLED.setBrightness(m_brightness);
  // clear the onboard led so it displays nothing
  clearOnboardLED();
  return true;
}

void LedControl::clearOnboardLED()
{
  // show nothing otherwise it might show random colours
  m_onboardLED.begin();
  m_onboardLED.show();
}

void LedControl::setIndex(int target, RGBColor col)
{
  m_leds[target] = col.raw_dword;
}

void LedControl::setRange(int first, int last, RGBColor col)
{
  for (int a = first; a <= last; a++) {
    setIndex(a, col);
  }
}

void LedControl::clearAll(RGBColor col)
{
  setRange(0, NUM_LEDS, col);
}

void LedControl::update()
{
  FastLED.show(m_brightness);
}
