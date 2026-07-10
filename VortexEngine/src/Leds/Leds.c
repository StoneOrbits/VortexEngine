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
#include <avr/io.h>
#include <string.h>
#endif

#define LED_DATA_PIN  7

uint8_t Leds_brightness = DEFAULT_BRIGHTNESS;
RGBColor Leds_ledColors[LED_COUNT] = { {0, 0, 0} };
volatile uint8_t *Leds_port = NULL;
uint8_t Leds_pinMask = 0;

bool Leds_init(void)
{
#ifdef VORTEX_EMBEDDED
  PORTB.DIRSET |= PIN4_bm;
  Leds_port = &VPORTB.OUT;
  Leds_pinMask = PIN4_bm;
#endif
#ifdef VORTEX_LIB
  Vortex_vcallbacks()->ledsInit(Leds_ledColors, LED_COUNT);
#endif
  return true;
}

void Leds_cleanup(void)
{
  for (uint8_t i = 0; i < LED_COUNT; ++i) {
    RGBColor_clear(&Leds_ledColors[i]);
  }
}

RGBColor* Leds_led(LedPos pos)
{
  if (pos > LED_LAST) {
    pos = LED_LAST;
  }
  return &Leds_ledColors[pos];
}

void Leds_setIndex(LedPos target, RGBColor col)
{
  if (target >= LED_COUNT) {
    Leds_setAll(col);
    return;
  }
  *Leds_led(target) = col;
}

void Leds_setRange(LedPos first, LedPos last, RGBColor col)
{
  for (LedPos pos = first; pos <= last; pos++) {
    Leds_setIndex(pos, col);
  }
}

void Leds_setAll(RGBColor col)
{
  Leds_setRange(LED_FIRST, LED_LAST, col);
}

void Leds_clearIndex(LedPos target)
{
  RGBColor off;
  RGBColor_initFromU32(&off, 0);
  Leds_setIndex(target, off);
}

void Leds_clearRange(LedPos first, LedPos last)
{
  RGBColor off;
  RGBColor_initFromU32(&off, 0);
  Leds_setRange(first, last, off);
}

void Leds_clearAll(void)
{
  RGBColor off;
  RGBColor_initFromU32(&off, 0);
  Leds_setAll(off);
}

void Leds_setPair(Pair pair, RGBColor col)
{
  Leds_setRange(pairEven(pair), pairOdd(pair), col);
}

void Leds_setPairs(Pair first, Pair last, RGBColor col)
{
  Leds_setRange(pairEven(first), pairOdd(last), col);
}

void Leds_clearPair(Pair pair)
{
  RGBColor off;
  RGBColor_initFromU32(&off, 0);
  Leds_setPair(pair, off);
}

void Leds_clearPairs(Pair first, Pair last)
{
  RGBColor off;
  RGBColor_initFromU32(&off, 0);
  Leds_setPairs(first, last, off);
}

void Leds_setRangeEvens(Pair first, Pair last, RGBColor col)
{
  for (Pair pos = first; pos <= last; pos++) {
    Leds_setIndex(pairEven(pos), col);
  }
}

void Leds_setAllEvens(RGBColor col)
{
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    Leds_setIndex(pairEven(pos), col);
  }
}

void Leds_setRangeOdds(Pair first, Pair last, RGBColor col)
{
  for (Pair pos = first; pos <= last; pos++) {
    Leds_setIndex(pairOdd(pos), col);
  }
}

void Leds_setAllOdds(RGBColor col)
{
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    Leds_setIndex(pairOdd(pos), col);
  }
}

void Leds_clearRangeEvens(Pair first, Pair last)
{
  for (Pair pos = first; pos <= last; pos++) {
    Leds_clearIndex(pairEven(pos));
  }
}

void Leds_clearAllEvens(void)
{
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    Leds_clearIndex(pairEven(pos));
  }
}

void Leds_clearRangeOdds(Pair first, Pair last)
{
  for (Pair pos = first; pos <= last; pos++) {
    Leds_clearIndex(pairOdd(pos));
  }
}

void Leds_clearAllOdds(void)
{
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    Leds_clearIndex(pairOdd(pos));
  }
}

void Leds_setMap(LedMap map, RGBColor col)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (ledmapCheckLed(map, pos)) {
      Leds_setIndex(pos, col);
    }
  }
}

void Leds_clearMap(LedMap map)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (ledmapCheckLed(map, pos)) {
      Leds_clearIndex(pos);
    }
  }
}

void Leds_stashAll(LedStash *stash)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    stash->ledColorsStash[pos] = *Leds_led(pos);
  }
}

void Leds_restoreAll(const LedStash *stash)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    *Leds_led(pos) = stash->ledColorsStash[pos];
  }
}

void Leds_adjustBrightnessIndex(LedPos target, uint8_t fadeBy)
{
  RGBColor_adjustBrightness(Leds_led(target), fadeBy);
}

void Leds_adjustBrightnessRange(LedPos first, LedPos last, uint8_t fadeBy)
{
  for (LedPos pos = first; pos <= last; pos++) {
    Leds_adjustBrightnessIndex(pos, fadeBy);
  }
}

void Leds_adjustBrightnessAll(uint8_t fadeBy)
{
  Leds_adjustBrightnessRange(LED_FIRST, LED_LAST, fadeBy);
}

void Leds_blinkIndexOffset(LedPos target, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((time % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    Leds_setIndex(target, col);
  }
}

void Leds_blinkRangeOffset(LedPos first, LedPos last, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((time % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    Leds_setRange(first, last, col);
  }
}

void Leds_blinkIndex(LedPos target, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time_getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    Leds_setIndex(target, col);
  }
}

void Leds_blinkRange(LedPos first, LedPos last, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time_getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    Leds_setRange(first, last, col);
  }
}

void Leds_blinkMap(LedMap targets, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time_getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; pos++) {
      if (ledmapCheckLed(targets, pos)) {
        Leds_setIndex(pos, col);
      }
    }
  }
}

void Leds_blinkAll(uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time_getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    Leds_setRange(LED_FIRST, LED_LAST, col);
  }
}

void Leds_blinkPair(Pair pair, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time_getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    Leds_setRange(pairEven(pair), pairOdd(pair), col);
  }
}

void Leds_blinkPairs(Pair first, Pair last, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time_getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    Leds_setRange(pairEven(first), pairOdd(last), col);
  }
}

void Leds_breatheIndex(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  HSVColor hsv;
  HSVColor_initHSV(&hsv, (uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val);
  RGBColor col;
  RGBColor_assignHSV(&col, &hsv);
  Leds_setIndex(target, col);
}

void Leds_breatheRange(LedPos first, LedPos last, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  HSVColor hsv;
  HSVColor_initHSV(&hsv, (uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val);
  RGBColor col;
  RGBColor_assignHSV(&col, &hsv);
  Leds_setRange(first, last, col);
}

void Leds_breatheIndexSat(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  HSVColor hsv;
  HSVColor_initHSV(&hsv, hue, 255 - (uint8_t)(sat + 128 + ((sin(variance * 0.0174533) + 1) * magnitude)), val);
  RGBColor col;
  RGBColor_assignHSV(&col, &hsv);
  Leds_setIndex(target, col);
}

void Leds_breatheIndexVal(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  HSVColor hsv;
  HSVColor_initHSV(&hsv, hue, sat, 255 - (uint8_t)(val + 128 + ((sin(variance * 0.0174533) + 1) * magnitude)));
  RGBColor col;
  RGBColor_assignHSV(&col, &hsv);
  Leds_setIndex(target, col);
}

void Leds_holdAll(RGBColor col)
{
  Leds_setAll(col);
  Leds_update();
  Time_delayMilliseconds(250);
}

RGBColor Leds_getLed(LedPos pos)
{
  return *Leds_led(pos);
}

uint8_t Leds_getBrightness(void)
{
  return Leds_brightness;
}

void Leds_setBrightness(uint8_t brightness)
{
  Leds_brightness = brightness;
}

void Leds_update(void)
{
#ifdef VORTEX_EMBEDDED
  RGBColor ledbackups[LED_COUNT];
  memcpy(ledbackups, Leds_ledColors, sizeof(Leds_ledColors));
  for (int c = 0; c < LED_COUNT; ++c) {
#define SCALE8(i, scale)  (((uint16_t)i * (uint16_t)(scale)) >> 8)
    Leds_ledColors[c].red = SCALE8(Leds_ledColors[c].red, Leds_brightness);
    Leds_ledColors[c].green = SCALE8(Leds_ledColors[c].green, Leds_brightness);
    Leds_ledColors[c].blue = SCALE8(Leds_ledColors[c].blue, Leds_brightness);
  }
  {
    uint8_t _tmp = Leds_ledColors[LED_1].red;
    Leds_ledColors[LED_1].red = Leds_ledColors[LED_1].green;
    Leds_ledColors[LED_1].green = _tmp;
  }
#endif

#ifdef VORTEX_LIB
  Vortex_vcallbacks()->ledsShow();
#endif

#ifdef VORTEX_EMBEDDED
  volatile uint16_t
    i = LED_COUNT * sizeof(RGBColor);
  volatile uint8_t
    *ptr = (volatile uint8_t *)Leds_ledColors,
    b = *ptr++,
    hi,
    lo;

  #if (F_CPU >= 9500000UL) && (F_CPU <= 11100000UL)
    volatile uint8_t next;

    hi   = *Leds_port |  Leds_pinMask;
    lo   = *Leds_port & ~Leds_pinMask;
    next = lo;
    if (b & 0x80) {
      next = hi;
    }

    asm volatile(
      "_head10:"                  "\n\t"
      "st   %a[port], %[hi]"      "\n\t"
      "rcall _bitTime10"          "\n\t"
      "st   %a[port], %[hi]"      "\n\t"
      "rcall _bitTime10"          "\n\t"
      "st   %a[port], %[hi]"      "\n\t"
      "rcall _bitTime10"          "\n\t"
      "st   %a[port], %[hi]"      "\n\t"
      "rcall _bitTime10"          "\n\t"
      "st   %a[port], %[hi]"      "\n\t"
      "rcall _bitTime10"          "\n\t"
      "st   %a[port], %[hi]"      "\n\t"
      "rcall _bitTime10"          "\n\t"
      "st   %a[port], %[hi]"      "\n\t"
      "rcall _bitTime10"          "\n\t"
      "st   %a[port], %[hi]"      "\n\t"
      "st   %a[port], %[next]"    "\n\t"
      "ld   %[byte] , %a[ptr]+"   "\n\t"
      "st   %a[port], %[next]"    "\n\t"
      "mov  %[next] , %[lo]"      "\n\t"
      "sbrc %[byte] , 7"          "\n\t"
       "mov %[next] , %[hi]"      "\n\t"
      "st   %a[port], %[lo]"      "\n\t"
      "sbiw %[count], 1"          "\n\t"
      "brne _head10"              "\n\t"
       "rjmp _done10"             "\n\t"
      "_bitTime10:"               "\n\t"
       "st   %a[port], %[next]"   "\n\t"
       "mov  %[next], %[lo]"      "\n\t"
       "lsl  %[byte]"             "\n\t"
       "sbrc %[byte], 7"          "\n\t"
        "mov %[next], %[hi]"      "\n\t"
       "st   %a[port], %[lo]"     "\n\t"
       "ret"                      "\n\t"
       "_done10:"                 "\n"
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (Leds_port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

#elif (F_CPU >= 19000000UL) && (F_CPU <= 22000000L)
    volatile uint8_t next, bit;

    hi   = *Leds_port |  Leds_pinMask;
    lo   = *Leds_port & ~Leds_pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head20:"                   "\n\t"
      "st   %a[port],  %[hi]"    "\n\t"
      "sbrc %[byte],  7"         "\n\t"
       "mov  %[next], %[hi]"     "\n\t"
      "dec  %[bit]"              "\n\t"
      "nop"                      "\n\t"
      "rjmp .+0"                 "\n\t"
      "st   %a[port],  %[next]"  "\n\t"
      "mov  %[next] ,  %[lo]"    "\n\t"
      "breq nextbyte20"          "\n\t"
      "rol  %[byte]"             "\n\t"
      "rjmp .+0"                 "\n\t"
      "rjmp .+0"                 "\n\t"
      "st   %a[port],  %[lo]"    "\n\t"
      "nop"                      "\n\t"
      "rjmp .+0"                 "\n\t"
      "rjmp .+0"                 "\n\t"
      "rjmp .+0"                 "\n\t"
      "rjmp head20"              "\n\t"
     "nextbyte20:"               "\n\t"
      "ldi  %[bit]  ,  8"        "\n\t"
      "ld   %[byte] ,  %a[ptr]+" "\n\t"
      "nop"                      "\n\t"
      "st   %a[port], %[lo]"     "\n\t"
      "nop"                      "\n\t"
      "rjmp .+0"                 "\n\t"
      "rjmp .+0"                 "\n\t"
      "sbiw %[count], 1"         "\n\t"
       "brne head20"             "\n"
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (Leds_port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

  #else
    #error "CPU SPEED NOT SUPPORTED"
  #endif
#endif

#ifdef VORTEX_EMBEDDED
  {
    uint8_t _tmp = Leds_ledColors[LED_1].red;
    Leds_ledColors[LED_1].red = Leds_ledColors[LED_1].green;
    Leds_ledColors[LED_1].green = _tmp;
  }
  memcpy(Leds_ledColors, ledbackups, sizeof(Leds_ledColors));
#endif
}
