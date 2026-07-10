#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>
#include <stdbool.h>
#include "../Colors/ColorTypes.h"
#include "LedTypes.h"

typedef struct LedStash_s LedStash;

bool Leds_init(void);
void Leds_cleanup(void);
void Leds_setIndex(LedPos target, RGBColor col);
void Leds_setRange(LedPos first, LedPos last, RGBColor col);
void Leds_setAll(RGBColor col);
void Leds_clearIndex(LedPos target);
void Leds_clearRange(LedPos first, LedPos last);
void Leds_clearAll(void);
void Leds_setPair(Pair pair, RGBColor col);
void Leds_setPairs(Pair first, Pair last, RGBColor col);
void Leds_clearPair(Pair pair);
void Leds_clearPairs(Pair first, Pair last);
void Leds_setRangeEvens(Pair first, Pair last, RGBColor col);
void Leds_setAllEvens(RGBColor col);
void Leds_setRangeOdds(Pair first, Pair last, RGBColor col);
void Leds_setAllOdds(RGBColor col);
void Leds_clearRangeEvens(Pair first, Pair last);
void Leds_clearAllEvens(void);
void Leds_clearRangeOdds(Pair first, Pair last);
void Leds_clearAllOdds(void);
void Leds_setMap(LedMap map, RGBColor col);
void Leds_clearMap(LedMap map);
void Leds_stashAll(LedStash *stash);
void Leds_restoreAll(const LedStash *stash);
void Leds_adjustBrightnessIndex(LedPos target, uint8_t fadeBy);
void Leds_adjustBrightnessRange(LedPos first, LedPos last, uint8_t fadeBy);
void Leds_adjustBrightnessAll(uint8_t fadeBy);
void Leds_blinkIndexOffset(LedPos target, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkRangeOffset(LedPos first, LedPos last, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkIndex(LedPos target, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkRange(LedPos first, LedPos last, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkMap(LedMap targets, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkAll(uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkPair(Pair pair, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkPairs(Pair first, Pair last, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_breatheIndex(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_breatheRange(LedPos first, LedPos last, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_breatheIndexSat(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_breatheIndexVal(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_holdAll(RGBColor col);
RGBColor Leds_getLed(LedPos pos);
uint8_t Leds_getBrightness(void);
void Leds_setBrightness(uint8_t brightness);
void Leds_update(void);

extern uint8_t Leds_brightness;
extern RGBColor Leds_ledColors[LED_COUNT];
extern volatile uint8_t *Leds_port;
extern uint8_t Leds_pinMask;

RGBColor* Leds_led(LedPos pos);

#endif
