#include "LedControl.h"

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#include "Settings.h"

#define LED_DATA_PIN  4

#define POWER_LED_PIN 7
#define POWER_LED_CLK 8

// should only be one LED control
LedControl *g_pLedControl = nullptr;

LedControl::LedControl() :
  m_ledColors(),
  m_onboardLED(1, POWER_LED_PIN, POWER_LED_CLK, DOTSTAR_BGR)
{
}

LedControl::~LedControl()
{
  g_pLedControl = nullptr;
}

bool LedControl::init()
{
  if (g_pLedControl) {
    // programmer error, only one led control
    return false;
  }
  g_pLedControl = this;
  // setup leds on data pin 4
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>((CRGB *)m_ledColors, LED_COUNT);
  FastLED.setBrightness(g_pSettings->getBrightness());
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
  m_ledColors[target] = col;
}

void LedControl::setRange(LedPos first, LedPos last, RGBColor col)
{
  for (LedPos pos = first; pos <= last; pos++) {
    setIndex(pos, col);
  }
}

void LedControl::setAll(RGBColor col)
{
  setRange(LED_FIRST, LED_LAST, col);
}

void LedControl::setFinger(Finger finger, RGBColor col)
{
  setRange(fingerTop(finger), fingerTip(finger), col);
}

void LedControl::setFingers(Finger first, Finger last, RGBColor col)
{
  setRange(fingerTop(first), fingerTip(last), col);
}

void LedControl::update()
{
  FastLED.show(g_pSettings->getBrightness());
}
