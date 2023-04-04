#include <math.h>

#include "LedStash.h"
#include "Leds.h"

#include "../Time/TimeControl.h"
#include "../Modes/Modes.h"

#ifdef VORTEX_LIB
#include "../../VortexLib/VortexLib.h"
#endif

// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };
// global brightness
uint32_t Leds::m_brightness = DEFAULT_BRIGHTNESS;

bool Leds::init()
{
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

void Leds::setPair(Pair pair, RGBColor col)
{
  // start from tip and go to top
  setRange(pairEven(pair), pairOdd(pair), col);
}

void Leds::setPairs(Pair first, Pair last, RGBColor col)
{
  // start from tip and go to top
  setRange(pairEven(first), pairOdd(last), col);
}

void Leds::setRangeEvens(Pair first, Pair last, RGBColor col)
{
  for (Pair pos = first; pos <= last; pos++) {
    setIndex(pairEven(pos), col);
  }
}

void Leds::setAllEvens(RGBColor col)
{
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    setIndex(pairEven(pos), col);
  }
}

void Leds::setRangeOdds(Pair first, Pair last, RGBColor col)
{
  for (Pair pos = first; pos <= last; pos++) {
    setIndex(pairOdd(pos), col);
  }
}

void Leds::setAllOdds(RGBColor col)
{
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    setIndex(pairOdd(pos), col);
  }
}

void Leds::clearRangeEvens(Pair first, Pair last) {
  for (Pair pos = first; pos <= last; pos++) {
    clearIndex(pairEven(pos));
  }
}

void Leds::clearAllEvens()
{
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    clearIndex(pairEven(pos));
  }
}

void Leds::clearRangeOdds(Pair first, Pair last) {
  for (Pair pos = first; pos <= last; pos++) {
    clearIndex(pairOdd(pos));
  }
}

void Leds::clearAllOdds()
{
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    clearIndex(pairOdd(pos));
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

void Leds::blinkAll(uint64_t time, int32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(LED_FIRST, LED_LAST, col);
  }
}

void Leds::blinkPair(Pair pair, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(pairEven(pair), pairOdd(pair), col);
  }
}

void Leds::blinkPairs(Pair first, Pair last, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(pairEven(first), pairOdd(last), col);
  }
}

void Leds::breathIndex(LedPos target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
}

void Leds::update()
{
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsShow();
#endif
}
