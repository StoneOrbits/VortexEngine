#include "LedControl.h"

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#include "TimeControl.h"
#include "Settings.h"

#define LED_DATA_PIN  4

#define POWER_LED_PIN 7
#define POWER_LED_CLK 8

// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };
// the onboard LED on the adafruit board
Adafruit_DotStar Leds::m_onboardLED(1, POWER_LED_PIN, POWER_LED_CLK, DOTSTAR_BGR);

bool Leds::init()
{
  // setup leds on data pin 4
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>((CRGB *)m_ledColors, LED_COUNT);
  FastLED.setBrightness(Settings::getBrightness());
  // clear the onboard led so it displays nothing
  clearOnboardLED();
  return true;
}

void Leds::clearOnboardLED()
{
  // show nothing otherwise it might show random colours
  m_onboardLED.begin();
  m_onboardLED.show();
}

void Leds::setIndex(LedPos target, RGBColor col)
{
  // safety
  if (target > LED_LAST) {
    target = LED_LAST;
  }
  // FLIP THE INDEXES because we want our enums to go from 
  // PINKIE to INDEX for sake of simple iteration in menus
  // but the current hardware configuration is flipped
  m_ledColors[LED_LAST - target] = col;
}

void Leds::setRange(LedPos first, LedPos last, RGBColor col)
{
  for (LedPos pos = first; pos <= last; pos++) {
    setIndex(pos, col);
  }
}

void Leds::setAll(RGBColor col)
{
  setRange(LED_FIRST, LED_LAST, col);
}

void Leds::setFinger(Finger finger, RGBColor col)
{
  // start from tip and go to top
  setRange(fingerTip(finger), fingerTop(finger), col);
}

void Leds::setFingers(Finger first, Finger last, RGBColor col)
{
  // start from tip and go to top
  setRange(fingerTip(first), fingerTop(last), col);
}

void Leds::blinkIndex(LedPos target, uint32_t offMs, RGBColor col)
{
  if ((Time::getCurtime() % Time::msToTicks(1000)) < Time::msToTicks(offMs)) {
    blinkIndexInternal(target, col);
  }
}

void Leds::blinkIndexInternal(LedPos target, RGBColor col)
{
  // blink to dark red if
  if (m_ledColors[LED_LAST - target].empty()) {
    col = RGBColor(150, 0, 0);
  }
  setIndex(target, col);
}

void Leds::blinkRangeInternal(LedPos first, LedPos last, RGBColor col)
{
  for (LedPos pos = first; pos <= last; ++pos) {
    blinkIndexInternal(pos, col);
  }
}

void Leds::blinkRange(LedPos first, LedPos last, uint32_t offMs, RGBColor col)
{
  if ((Time::getCurtime() % Time::msToTicks(1000)) < Time::msToTicks(offMs)) {
    blinkRangeInternal(first, last, col);
  }
}

void Leds::blinkAll(int32_t offMs, RGBColor col)
{
  if ((Time::getCurtime() % Time::msToTicks(1000)) < Time::msToTicks(offMs)) {
    blinkRangeInternal(LED_FIRST, LED_LAST, col);
  }
}

void Leds::blinkFinger(Finger finger, uint32_t offMs, RGBColor col)
{
  if ((Time::getCurtime() % Time::msToTicks(1000)) < Time::msToTicks(offMs)) {
    blinkRangeInternal(fingerTip(finger), fingerTop(finger), col);
  }
}

void Leds::blinkFingers(Finger first, Finger last, uint32_t offMs, RGBColor col)
{
  if ((Time::getCurtime() % Time::msToTicks(1000)) < Time::msToTicks(offMs)) {
    blinkRangeInternal(fingerTip(first), fingerTop(last), col);
  }
}

void Leds::update()
{
  FastLED.show(Settings::getBrightness());
}
