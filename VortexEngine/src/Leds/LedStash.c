#include "LedStash.h"

void LedStash_init(LedStash *self)
{
  (void)self;
}

void LedStash_setIndex(LedStash *self, LedPos pos, RGBColor col)
{
  if (pos > LED_LAST) {
    return;
  }
  self->ledColorsStash[pos] = col;
}

void LedStash_clear(LedStash *self)
{
  for (LedPos pos = LED_FIRST; pos < LED_LAST; ++pos) {
    RGBColor_clear(&self->ledColorsStash[pos]);
  }
}

RGBColor* LedStash_get(LedStash *self, int index)
{
  return &self->ledColorsStash[index];
}

void LedStash_cleanup(LedStash *self)
{
  (void)self;
}
