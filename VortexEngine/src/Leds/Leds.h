#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>
#include <stdbool.h>

#include "../Colors/ColorTypes.h"
#include "LedTypes.h"

typedef struct LedStash LedStash;

// Leds is a stateless namespace of free functions, there should only ever
// be one instance of it and the state lives in Leds.cpp

// opting for static class here because there should only ever be one
// Led control object and I don't like singletons
bool Leds_init(void);
void Leds_cleanup(void);

// control individual LED, these are appropriate to use in internal pattern logic
void Leds_setIndex(LedPos target, RGBColor col);
void Leds_setRange(LedPos first, LedPos last, RGBColor col);
void Leds_setAll(RGBColor col);

// Turn off individual LEDs, these are appropriate to use in internal pattern logic
static inline void Leds_clearIndex(LedPos target) { Leds_setIndex(target, RGB_OFF); }
static inline void Leds_clearRange(LedPos first, LedPos last) { Leds_setRange(first, last, RGB_OFF); }
static inline void Leds_clearAll(void) { Leds_setAll(RGB_OFF); }

// control two LEDs on a pair, these are appropriate for use in internal pattern logic
void Leds_setPair(Pair pair, RGBColor col);
void Leds_setPairs(Pair first, Pair last, RGBColor col);

// Turn off both LEDs on a pair, these are appropriate to use in internal pattern logic
static inline void Leds_clearPair(Pair pair) { Leds_setPair(pair, RGB_OFF); }
static inline void Leds_clearPairs(Pair first, Pair last) { Leds_setPairs(first, last, RGB_OFF); }

// Controll pair evens
void Leds_setRangeEvens(Pair first, Pair last, RGBColor col);
void Leds_setAllEvens(RGBColor col);
// Controll pair odds
void Leds_setRangeOdds(Pair first, Pair last, RGBColor col);
void Leds_setAllOdds(RGBColor col);

// Turn off tips
void Leds_clearRangeEvens(Pair first, Pair last);
void Leds_clearAllEvens(void);
// Turn off tops
void Leds_clearRangeOdds(Pair first, Pair last);
void Leds_clearAllOdds(void);

// Turn on/off a mapping of leds with a color
void Leds_setMap(LedMap map, RGBColor col);
void Leds_clearMap(LedMap map);

// stores Led for later use
void Leds_stashAll(LedStash *stash);

// restores Leds from stash
void Leds_restoreAll(const LedStash *stash);

// Dim individual LEDs, these are appropriate to use in internal pattern logic
void Leds_adjustBrightnessIndex(LedPos target, uint8_t fadeBy);
void Leds_adjustBrightnessRange(LedPos first, LedPos last, uint8_t fadeBy);
void Leds_adjustBrightnessAll(uint8_t fadeBy);

// Blink an led to blank or a color
//
// These APIs work by checking if the current time is within the
// 'on duration' then they apply the color to the target, otherwise they do
// nothing. The goal of this behaviour is to allow them to be used in any
// situation to 'blink' an led to either off or some color.
//
// However since these APIs modulate current time to check if within the 'on'
// threshold that makes them unsuitable for internal pattern usage because it
// is unpredictable whether they will blink on or off first
void Leds_blinkIndexOffset(LedPos target, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkRangeOffset(LedPos first, LedPos last, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkIndex(LedPos target, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkRange(LedPos first, LedPos last, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkMap(LedMap targets, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkAll(uint16_t offMs, uint16_t onMs, RGBColor col);
// Blink both LEDs on a pair
void Leds_blinkPair(Pair pair, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkPairs(Pair first, Pair last, uint16_t offMs, uint16_t onMs, RGBColor col);

// breath the hue on an index
// warning: these use hsv to rgb in realtime!
void Leds_breatheIndex(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_breatheRange(LedPos first, LedPos last, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_breatheIndexSat(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_breatheIndexVal(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);

// a very specialized api to hold all leds on a color for 250ms
void Leds_holdAll(RGBColor col);

// get the RGBColor of an Led index
RGBColor *Leds_ledRef(LedPos pos);
static inline RGBColor Leds_led(LedPos pos) { return *Leds_ledRef(pos); }
static inline RGBColor Leds_getLed(LedPos pos) { return Leds_led(pos); }

// global brightness
uint8_t Leds_getBrightness(void);
void Leds_setBrightness(uint8_t brightness);

// actually update the LEDs and show the changes
void Leds_update(void);

// the default blink/breathe parameters, kept here so the arity macros
// below can supply them when the caller omits trailing arguments
#define LED_BLINK_OFF_MS   250
#define LED_BLINK_ON_MS    500
#define LED_BREATHE_MAG    15
#define LED_BREATHE_SAT    255
#define LED_BREATHE_VAL    210

// arity dispatch for the trailing-default blink/breathe helpers, the
// indirection through _() macros is required so LED_NARG() expands
// before being pasted with ##
#define LED_NARG(_1, _2, _3, _4, _5, _6, N, ...) N
#define LED_COUNT(...) LED_NARG(__VA_ARGS__, 6, 5, 4, 3, 2, 1)
#define LED_CAT_(a, b) a##b
#define LED_DISPATCH_(prefix, N, ...) LED_CAT_(prefix, N)(__VA_ARGS__)

// Leds_blinkIndex(t[, offMs[, onMs[, col]]])
#define Leds_blinkIndex(...) LED_DISPATCH_(Leds_blinkIndex, LED_COUNT(__VA_ARGS__), __VA_ARGS__)
void Leds_blinkIndex1(LedPos target);
void Leds_blinkIndex2(LedPos target, uint16_t offMs);
void Leds_blinkIndex3(LedPos target, uint16_t offMs, uint16_t onMs);
void Leds_blinkIndex4(LedPos target, uint16_t offMs, uint16_t onMs, RGBColor col);

// Leds_blinkAll([offMs[, onMs[, col]]])
#define Leds_blinkAll(...) LED_DISPATCH_(Leds_blinkAll, LED_COUNT(__VA_ARGS__), __VA_ARGS__)
void Leds_blinkAll0(void);
void Leds_blinkAll1(uint16_t offMs);
void Leds_blinkAll2(uint16_t offMs, uint16_t onMs);
void Leds_blinkAll3(uint16_t offMs, uint16_t onMs, RGBColor col);

// Leds_blinkIndexOffset(t, time[, offMs[, onMs[, col]]])
#define Leds_blinkIndexOffset(...) LED_DISPATCH_(Leds_blinkIndexOffset, LED_COUNT(__VA_ARGS__), __VA_ARGS__)
void Leds_blinkIndexOffset2(LedPos target, uint32_t time);
void Leds_blinkIndexOffset3(LedPos target, uint32_t time, uint16_t offMs);
void Leds_blinkIndexOffset4(LedPos target, uint32_t time, uint16_t offMs, uint16_t onMs);
void Leds_blinkIndexOffset5(LedPos target, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col);

// Leds_breatheIndex(t, hue, variance[, magnitude[, sat[, val]]])
#define Leds_breatheIndex(...) LED_DISPATCH_(Leds_breatheIndex, LED_COUNT(__VA_ARGS__), __VA_ARGS__)
void Leds_breatheIndex3(LedPos target, uint8_t hue, uint32_t variance);
void Leds_breatheIndex4(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude);
void Leds_breatheIndex5(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat);
void Leds_breatheIndex6(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);

// Leds_breatheIndexSat / Leds_breatheIndexVal share the same shape
#define Leds_breatheIndexSat(...) LED_DISPATCH_(Leds_breatheIndexSat, LED_COUNT(__VA_ARGS__), __VA_ARGS__)
void Leds_breatheIndexSat3(LedPos target, uint8_t hue, uint32_t variance);
void Leds_breatheIndexSat4(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude);
void Leds_breatheIndexSat5(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat);
void Leds_breatheIndexSat6(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);

#define Leds_breatheIndexVal(...) LED_DISPATCH_(Leds_breatheIndexVal, LED_COUNT(__VA_ARGS__), __VA_ARGS__)
void Leds_breatheIndexVal3(LedPos target, uint8_t hue, uint32_t variance);
void Leds_breatheIndexVal4(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude);
void Leds_breatheIndexVal5(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat);
void Leds_breatheIndexVal6(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);

#endif
