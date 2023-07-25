#include <math.h>

#include "LedStash.h"
#include "Leds.h"

#include "../Time/TimeControl.h"
#include "../Modes/Modes.h"

#include "../VortexConfig.h"

#ifdef VORTEX_LIB
#include <Arduino.h>
#include "../../VortexLib/VortexLib.h"
#endif

#ifdef VORTEX_EMBEDDED
#include <FastLED.h>

#define LED_DATA_PIN  4
#define CLOCK_PIN 3

// TODO: remove this and just set the pins to INPUT
// onboard LED on adafruit
#include <Adafruit_DotStar.h>
#define POWER_LED_PIN 7
#define POWER_LED_CLK 8
Adafruit_DotStar onboardLED(1, POWER_LED_PIN, POWER_LED_CLK, DOTSTAR_BGR);
#endif

// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };
// global brightness
uint8_t Leds::m_brightness = DEFAULT_BRIGHTNESS;

bool Leds::init()
{
#ifdef VORTEX_EMBEDDED
  // setup leds on data pin 4
  FastLED.addLeds<DOTSTAR, LED_DATA_PIN, CLOCK_PIN, BGR>((CRGB *)m_ledColors, LED_COUNT);
  // get screwed fastled, don't throttle us!
  FastLED.setMaxRefreshRate(0, false);
  // clear the onboard led so it displays nothing
  onboardLED.begin();
  onboardLED.show();
#endif
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsInit(m_ledColors, LED_COUNT);
#endif
  return true;
}

void Leds::cleanup()
{
  for (uint8_t i = 0; i < LED_COUNT; ++i) {
    m_ledColors[i].clear();
  }
}

void Leds::setIndex(LedPos target, RGBColor col)
{
  if (target >= LED_COUNT) {
    setAll(col);
    return;
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

void Leds::setPair(Pair pair, RGBColor col)
{
  setIndex(pairTop(pair), col);
  setIndex(pairBot(pair), col);
}

void Leds::setPairs(Pair first, Pair last, RGBColor col)
{
  // start from tip and go to top
  for (Pair p = first; p < last; ++p) {
    setPair(p, col);
  }
}

void Leds::setAllEvens(RGBColor col)
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    if ((i % 2) == 0) {
      setIndex(i, col);
    }
  }
}

void Leds::setAllOdds(RGBColor col)
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    if ((i % 2) != 0) {
      setIndex(i, col);
    }
  }
}

void Leds::clearAllEvens()
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    if ((i % 2) == 0) {
      clearIndex(i);
    }
  }
}

void Leds::clearAllOdds()
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    if ((i % 2) != 0) {
      clearIndex(i);
    }
  }
}

void Leds::setQuadrant(Quadrant quadrant, RGBColor col)
{
  // start from tip and go to top
  setRange(quadrantFirstLed(quadrant), quadrantLastLed(quadrant), col);
}

void Leds::setQuadrants(Quadrant first, Quadrant last, RGBColor col)
{
  // start from tip and go to top
  setRange(quadrantFirstLed(first), quadrantLastLed(last), col);
}

void Leds::setQuadrantFive(RGBColor col)
{
  led(LED_3) = col;
  led(LED_10) = col;
  led(LED_17) = col;
  led(LED_24) = col;
}

void Leds::clearQuadrantFive()
{
  led(LED_3) = RGB_OFF;
  led(LED_10) = RGB_OFF;
  led(LED_17) = RGB_OFF;
  led(LED_24) = RGB_OFF;
}

void Leds::setRing(Ring ring, RGBColor col)
{
  for (Pair i = PAIR_FIRST; i < 4; ++i) {
    setPair((Pair)(ring + (4 * i)), col);
  }
}

void Leds::setRings(Ring first, Ring last, RGBColor col)
{
  for (Ring i = first; i <= last; ++i) {
    setRing(i, col);
  }
}

void Leds::clearRing(Ring ring)
{
  for (Pair i = PAIR_FIRST; i < 4; ++i) {
    clearPair((Pair)(ring + (4 * i)));
  }
}

void Leds::clearRings(Ring first, Ring last)
{
  for (Ring i = first; i <= last; ++i) {
    clearRing(i);
  }
}

void Leds::setMap(LedMap map, RGBColor col)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (checkLed(map, pos)) {
      setIndex(pos, col);
    }
  }
}

void Leds::clearMap(LedMap map)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (checkLed(map, pos)) {
      clearIndex(pos);
    }
  }
}

void Leds::stashAll(LedStash &stash)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    stash.m_ledColorsStash[pos] = led(pos);
  }
}

void Leds::restoreAll(const LedStash &stash)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    led(pos) = stash.m_ledColorsStash[pos];
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

// blinkIndex with the additional optional time offset
void Leds::blinkIndexOffset(LedPos target, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((time % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setIndex(target, col);
  }
}

void Leds::blinkIndex(LedPos target, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setIndex(target, col);
  }
}

void Leds::blinkRange(LedPos first, LedPos last, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setRange(first, last, col);
  }
}

void Leds::blinkQuadrantOffset(Quadrant target, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((time % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setQuadrant(target, col);
  }
}

void Leds::blinkQuadrant(Quadrant target, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setQuadrant(target, col);
  }
}

void Leds::blinkQuadrantFive(uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setQuadrantFive(col);
  }
}

void Leds::blinkMap(LedMap targets, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; pos++) {
      if (checkLed(targets, pos)) {
        setIndex(pos, col);
      }
    }
  }
}

void Leds::blinkAll(uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setRange(LED_FIRST, LED_LAST, col);
  }
}

void Leds::blinkPair(Pair pair, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setRange(pairEven(pair), pairOdd(pair), col);
  }
}

void Leds::blinkPairs(Pair first, Pair last, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setRange(pairEven(first), pairOdd(last), col);
  }
}

void Leds::breathIndex(LedPos target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
}

void Leds::breathIndexSat(LedPos target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor(hue, 255 - (uint8_t)(sat + 128 + ((sin(variance * 0.0174533) + 1) * magnitude)), val));
}

void Leds::breathIndexVal(LedPos target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor(hue, sat, 255 - (uint8_t)(val + 128 + ((sin(variance * 0.0174533) + 1) * magnitude))));
}

void Leds::breathQuadrant(Quadrant target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  for (uint8_t pos = 0; pos < 7; ++pos) {
    setIndex((LedPos)((target * 7) + pos), HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
  }
}

void Leds::breathQuadrantFive(uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  for (int target = 0; target < 4; ++target) {
    setIndex((LedPos)((target * 7) + 3), HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
  }
}

void Leds::holdAll(RGBColor col)
{
  setAll(col);
  update();
  Time::delayMilliseconds(250);
}

void Leds::update()
{
#ifdef VORTEX_EMBEDDED
  FastLED.show(m_brightness);
#endif
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsShow();
#endif
}
