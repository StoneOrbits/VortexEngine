#include "LedControl.h"

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#define LED_DATA_PIN  4

#define POWER_LED_PIN 7
#define POWER_LED_CLK 8

using namespace std;

LedControl::LedControl() :
  m_ledColors(),
  m_brightness(255),
  m_onboardLED(1, POWER_LED_PIN, POWER_LED_CLK, DOTSTAR_BGR)
{
}

bool LedControl::init()
{
  // create the array of LEDS
  memset(m_ledColors, 0, sizeof(m_ledColors));
  // setup leds on data pin 4
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(m_ledColors, LED_COUNT);
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

void LedControl::setIndex(LedPos target, RGBColor col)
{
  m_ledColors[target] = col.raw_dword;
}

void LedControl::setRange(LedPos first, LedPos last, RGBColor col)
{
  for (LedPos pos = first; pos <= last; pos++) {
    setIndex(pos, col);
  }
}

void LedControl::clearAll(RGBColor col)
{
  setRange(0, LED_COUNT, col);
}

void LedControl::update()
{
  FastLED.show(m_brightness);
}
