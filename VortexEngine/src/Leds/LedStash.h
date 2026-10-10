#ifndef LEDSTASH_H
#define LEDSTASH_H

#include "../Colors/ColorTypes.h"
#include "LedTypes.h"

typedef struct LedStash {
  RGBColor m_ledColorsStash[LED_COUNT];
} LedStash;

void LedStash_make(LedStash *self);
void LedStash_destroy(LedStash *self);

void LedStash_setIndex(LedStash *self, LedPos pos, RGBColor col);

void LedStash_clear(LedStash *self);

// index operator to access color index
RGBColor *LedStash_at(LedStash *self, int index);

#endif
