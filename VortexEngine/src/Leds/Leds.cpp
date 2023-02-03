#include <math.h>

#include <Arduino.h>

#include "Leds.h"

#include "../Time/TimeControl.h"

#define LED_DATA_PIN  9

#define POWER_LED_PIN 7
#define POWER_LED_CLK 8

// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };
// the onboard LED on the adafruit board
//Adafruit_DotStar Leds::m_onboardLED(1, POWER_LED_PIN, POWER_LED_CLK, DOTSTAR_BGR);
// global brightness
uint32_t Leds::m_brightness = DEFAULT_BRIGHTNESS;

#ifdef TEST_FRAMEWORK
tinyNeoPixel Leds::m_pixels;
#else
tinyNeoPixel Leds::m_pixels = tinyNeoPixel(LED_COUNT, LED_DATA_PIN, NEO_GRB, (byte *)m_ledColors);
#endif

bool Leds::init()
{
  // setup leds on data pin 4
  //FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>((CRGB *)m_ledColors, LED_COUNT);
  // get screwed fastled, don't throttle us!
  //FastLED.setMaxRefreshRate(0, false);
  // clear the onboard led so it displays nothing
  // tiny neo pixels
#ifdef TEST_FRAMEWORK
  m_pixels = tinyNeoPixel(LED_COUNT, LED_DATA_PIN, NEO_RGB + NEO_KHZ800, m_ledColors);
#endif
  pinMode(LED_DATA_PIN, OUTPUT);
  return true;
}

void Leds::cleanup()
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    m_ledColors[i].clear();
  }
}

void Leds::setIndex(LedPos target, RGBColor col)
{
  // safety
  if (target > LED_LAST) {
    target = LED_LAST;
  }
  led(target) = col;
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

void Leds::setRangeTips(Finger first, Finger last, RGBColor col)
{
  for (Finger pos = first; pos <= last; pos++) {
    setIndex(fingerTip(pos), col);
  }
}

void Leds::setAllTips(RGBColor col)
{
  for (Finger pos = FINGER_FIRST; pos <= FINGER_LAST; pos++) {
    setIndex(fingerTip(pos), col);
  }
}

void Leds::setRangeTops(Finger first, Finger last, RGBColor col)
{
  for (Finger pos = first; pos <= last; pos++) {
    setIndex(fingerTop(pos), col);
  }
}

void Leds::setAllTops(RGBColor col)
{
  for (Finger pos = FINGER_FIRST; pos <= FINGER_LAST; pos++) {
    setIndex(fingerTop(pos), col);
  }
}

void Leds::clearRangeTips(Finger first, Finger last) {
  for (Finger pos = first; pos <= last; pos++) {
    clearIndex(fingerTip(pos));
  }
}

void Leds::clearAllTips()
{
  for (Finger pos = FINGER_FIRST; pos <= FINGER_LAST; pos++) {
    clearIndex(fingerTip(pos));
  }
}

void Leds::clearRangeTops(Finger first, Finger last) {
  for (Finger pos = first; pos <= last; pos++) {
    clearIndex(fingerTop(pos));
  }
}

void Leds::clearAllTops()
{
  for (Finger pos = FINGER_FIRST; pos <= FINGER_LAST; pos++) {
    clearIndex(fingerTop(pos));
  }
}

void Leds::adjustBrightnessIndex(LedPos target, uint8_t fadeBy)
{
   led(target).adjustBrightness(fadeBy);
}

void Leds::adjustBrightnessRange(LedPos first, LedPos last, uint8_t fadeBy)
{
  for (LedPos pos = first; pos <= last; pos++) {
    adjustBrightnessIndex(pos, fadeBy);
  }
}

void Leds::adjustBrightnessAll(uint8_t fadeBy)
{
  adjustBrightnessRange(LED_FIRST, LED_LAST, fadeBy);
}

void Leds::blinkIndex(LedPos target, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setIndex(target, col);
  }
}

void Leds::blinkRange(LedPos first, LedPos last, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(first, last, col);
  }
}

void Leds::blinkAll(uint64_t time, int32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(LED_FIRST, LED_LAST, col);
  }
}

void Leds::blinkFinger(Finger finger, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(fingerTip(finger), fingerTop(finger), col);
  }
}

void Leds::blinkFingers(Finger first, Finger last, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(fingerTip(first), fingerTop(last), col);
  }
}

void Leds::breathIndex(LedPos target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
}

void Leds::update()
{
  m_pixels.show();
}
