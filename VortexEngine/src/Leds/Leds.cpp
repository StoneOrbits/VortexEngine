#include <FastLED.h>
#include <Adafruit_DotStar.h>
#include <math.h>

#include "LedStash.h"
#include "Leds.h"

#include "../Time/TimeControl.h"
#include "../Modes/Modes.h"

#ifdef VORTEX_LIB
#include "../../VortexLib/VortexLib.h"
#endif

#define LED_DATA_PIN  4

#define POWER_LED_PIN 7
#define POWER_LED_CLK 8

#define CLOCK_PIN 3

// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };
// the onboard LED on the adafruit board
Adafruit_DotStar Leds::m_onboardLED(1, POWER_LED_PIN, POWER_LED_CLK, DOTSTAR_BGR);
// global brightness
uint32_t Leds::m_brightness = DEFAULT_BRIGHTNESS;

bool Leds::init()
{
  // setup leds on data pin 4
  FastLED.addLeds<DOTSTAR, LED_DATA_PIN, CLOCK_PIN, BGR>((CRGB *)m_ledColors, LED_COUNT);
  // get screwed fastled, don't throttle us!
  FastLED.setMaxRefreshRate(0, false);
  // clear the onboard led so it displays nothing
  clearOnboardLED();
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsInit(m_ledColors, LED_COUNT);
#endif
  return true;
}

void Leds::cleanup()
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    m_ledColors[i].clear();
  }
}

void Leds::clearOnboardLED()
{
  // show nothing otherwise it might show random colours
  m_onboardLED.begin();
  m_onboardLED.show();
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
  for (LedPos q = LED_FIRST; q < 4; ++q) {
    led((LedPos)((q * 7) + 3)) = col;
  }
}

void Leds::clearQuadrantFive()
{
  for (LedPos q = LED_FIRST; q < 4; ++q) {
    led((LedPos)((q * 7) + 3)) = HSV_OFF;
  }
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

void Leds::blinkQuadrant(Quadrant target, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setQuadrant(target, col);
  }
}

void Leds::blinkQuadrantFive( uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setQuadrantFive(col);
  }
}

void Leds::blinkAll(uint64_t time, int32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(LED_FIRST, LED_LAST, col);
  }
}

void Leds::blinkPair(Pair pair, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(pairTop(pair), pairBot(pair), col);
  }
}

void Leds::blinkPairs(Pair first, Pair last, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(pairTop(first), pairBot(last), col);
  }
}

void Leds::breathIndex(LedPos target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
}

void Leds::breathIndexSat(LedPos target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor(hue, 255 - (uint8_t)(sat + ((sin(variance * 0.0174533) + 1) * magnitude)), val));
}

void Leds::breathIndexVal(LedPos target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor(hue, sat, 255 - (uint8_t)(val + ((sin(variance * 0.0174533) + 1) * magnitude))));
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

void Leds::update()
{
  FastLED.show(m_brightness);
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsShow();
#endif
}