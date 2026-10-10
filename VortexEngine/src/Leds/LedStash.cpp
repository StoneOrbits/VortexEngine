#include "LedStash.h"

#include <string.h>

void LedStash_make(LedStash *self)
{
  memset((void *)self->m_ledColorsStash, 0, sizeof(self->m_ledColorsStash));
}

void LedStash_setIndex(LedStash *self, LedPos pos, RGBColor col)
{
  if (pos > LED_LAST) {
    return;
  }
  self->m_ledColorsStash[pos] = col;
}

void LedStash_clear(LedStash *self)
{
  for (LedPos pos = LED_FIRST; pos < LED_LAST; ++pos) {
    RGBColor_clear(&self->m_ledColorsStash[pos]);
  }
}

// index operator to access color index
RGBColor *LedStash_at(LedStash *self, int index)
{
  return &self->m_ledColorsStash[index];
}

void LedStash_destroy(LedStash *self)
{
}
