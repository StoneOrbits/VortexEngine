#ifndef LEDSTASH_H
#define LEDSTASH_H

#include "../Colors/ColorTypes.h"
#include "LedTypes.h"

typedef struct LedStash_s {
  RGBColor ledColorsStash[LED_COUNT];
} LedStash;

void LedStash_init(LedStash *self);
void LedStash_setIndex(LedStash *self, LedPos pos, RGBColor col);
void LedStash_clear(LedStash *self);
RGBColor* LedStash_get(LedStash *self, int index);
void LedStash_cleanup(LedStash *self);

#endif
