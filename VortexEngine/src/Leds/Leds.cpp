#include <math.h>

#include "LedStash.h"
#include "Leds.h"

#include "../Time/TimeControl.h"
#include "../Modes/Modes.h"

#include "../VortexConfig.h"

#ifdef VORTEX_LIB
#include "../../VortexLib/VortexLib.h"
#endif

#ifdef VORTEX_EMBEDDED
#include <Arduino.h>
#define LED_DATA_PIN  8
#endif

// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };
// global brightness
uint8_t Leds::m_brightness = DEFAULT_BRIGHTNESS;

bool Leds::init()
{
#ifdef VORTEX_EMBEDDED
  //for (int i = 0; i < 32; ++i) {
  //  pinMode(i, OUTPUT);
  //}
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

void Leds::clearRangeEvens(Pair first, Pair last)
{
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

void Leds::clearRangeOdds(Pair first, Pair last)
{
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

void Leds::breathIndex(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
}

void Leds::breathIndexSat(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor(hue, 255 - (uint8_t)(sat + 128 + ((sin(variance * 0.0174533) + 1) * magnitude)), val));
}

void Leds::breathIndexVal(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor(hue, sat, 255 - (uint8_t)(val + 128 + ((sin(variance * 0.0174533) + 1) * magnitude))));
}

void Leds::holdAll(RGBColor col)
{
  setAll(col);
  update();
  Time::delayMilliseconds(250);
}

#ifdef VORTEX_EMBEDDED

// idk how correct this is but it works
#define CPU_FREQUENCY_MHZ 240
#define NS_TO_CYCLES(ns) (((ns) * CPU_FREQUENCY_MHZ) / 1000)

// these timings were found through trial and error
#define T0H 1
#define T0L 50
#define T1H 50
#define T1L 1

inline void delay_loop(uint32_t loop)
{
  while (loop--) {
    __asm__ __volatile__("nop");
  }
}

inline void sendBit(bool bitVal)
{
  uint32_t pinMask = (1ul << LED_DATA_PIN); // Assuming LED_DATA_PIN is the GPIO pin number
  GPIO.out_w1ts = pinMask; // Set the output bit using GPIO.out_w1ts register
  delay_loop(NS_TO_CYCLES(bitVal ? T1H : T0H)); // Delay for T1H or T0H

  GPIO.out_w1tc = pinMask; // Clear the output bit using GPIO.out_w1tc register
  delay_loop(NS_TO_CYCLES(bitVal ? T0L : T1L)); // Delay for T0L or T1L
}

inline void sendByte(unsigned char byte)
{
  for (unsigned char bit = 0; bit < 8; bit++) {
    sendBit((byte & 0x80) != 0); // Neopixel wants bit in highest-to-lowest order
    byte <<= 1;                   // Shift left so bit 6 moves into 7, 5 moves into 6, etc
  }
}
#endif

void Leds::update()
{
#ifdef VORTEX_EMBEDDED
  //// Important: need to disable interrupts during the transmission
  //noInterrupts();
  //for (LedPos pos = LED_FIRST; pos < LED_COUNT; pos++) {
  //  const RGBColor &col = m_ledColors[pos];
  //  sendByte((col.green * m_brightness) >> 8);
  //  sendByte((col.red * m_brightness) >> 8);
  //  sendByte((col.blue * m_brightness) >> 8);
  //}
  //// Re-enable interrupts
  //interrupts();
  //// Required to latch the LEDs
  //delayMicroseconds(10);
  //for (int i = 0; i < 32; ++i) {
  //  digitalWrite(i, HIGH);
  //}
#endif
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsShow();
#endif
}
