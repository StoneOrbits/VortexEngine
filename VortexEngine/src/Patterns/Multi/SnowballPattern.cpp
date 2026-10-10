#include "SnowballPattern.h"

#include <string.h>
#include "../../Leds/Leds.h"

#define WORM_SIZE 6

#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable SnowballPattern_bvtable = {
  .base = {
    .destroy = SnowballPattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = SnowballPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = SnowballPattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = SnowballPattern_poststep,
};

void SnowballPattern_init(SnowballPattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &SnowballPattern_bvtable);
  self->m_progress = 0;
  Pattern_setPatternID((&self->base.base.base), PATTERN_SNOWBALL);
  Pattern_setArgs((&self->base.base.base), args);
}

void SnowballPattern_destroy(Pattern *pat)
{
  SnowballPattern *self = (SnowballPattern *)pat;
  (void)self;
}

void SnowballPattern_initImpl(Pattern *pat)
{
  SnowballPattern *self = (SnowballPattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // start at index 1
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 1);
}

void SnowballPattern_blinkOn(BlinkStepPattern *bsp)
{
  SnowballPattern *self = (SnowballPattern *)bsp;
  Leds_setAll(Colorset_get(&(&self->base.base.base)->m_colorset, 0));
  for (int body = 0; body < WORM_SIZE; ++body) {
    if (body + self->m_progress < LED_COUNT) {
      Leds_setIndex((LedPos)(body + self->m_progress), Colorset_cur(&(&self->base.base.base)->m_colorset));
    } else {
      RGBColor col = Colorset_peekNext(&(&self->base.base.base)->m_colorset);
      if (Colorset_curIndex(&(&self->base.base.base)->m_colorset) == Colorset_numColors(&(&self->base.base.base)->m_colorset) - 1) {
        col = Colorset_peek(&(&self->base.base.base)->m_colorset, 2);
      }
      Leds_setIndex((LedPos)((body + self->m_progress) % LED_COUNT), col);
    }
  }
}

void SnowballPattern_poststep(BlinkStepPattern *bsp)
{
  SnowballPattern *self = (SnowballPattern *)bsp;
  Colorset_skip(&(&self->base.base.base)->m_colorset, 1);
  if (Colorset_curIndex(&(&self->base.base.base)->m_colorset) == 0) {
    self->m_progress = (self->m_progress + 1) % LED_COUNT;
    Colorset_skip(&(&self->base.base.base)->m_colorset, 1);
  }
}
