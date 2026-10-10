#include "DripPattern.h"

#include <string.h>
#include "../../Leds/Leds.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable DripPattern_bvtable = {
  .base = {
    .destroy = DripPattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = DripPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = DripPattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = DripPattern_poststep,
};

void DripPattern_init(DripPattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &DripPattern_bvtable);
  Pattern_setPatternID((&self->base.base.base), PATTERN_DRIP);
  Pattern_setArgs((&self->base.base.base), args);
}

void DripPattern_destroy(Pattern *pat)
{
  DripPattern *self = (DripPattern *)pat;
  (void)self;
}

void DripPattern_initImpl(Pattern *pat)
{
  DripPattern *self = (DripPattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // reset sync bool
  self->m_sync = true;
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 0);
}

void DripPattern_blinkOn(BlinkStepPattern *bsp)
{
  DripPattern *self = (DripPattern *)bsp;
  if (!self->m_sync) {
    Leds_setAllEvens(Colorset_cur(&(&self->base.base.base)->m_colorset));
    Leds_setAllOdds(Colorset_peekNext(&(&self->base.base.base)->m_colorset));
  } else {
    Leds_setAll(Colorset_cur(&(&self->base.base.base)->m_colorset));
  }
}

void DripPattern_poststep(BlinkStepPattern *bsp)
{
  DripPattern *self = (DripPattern *)bsp;
  self->m_sync = !self->m_sync;
  if (self->m_sync) {
    Colorset_getNext(&(&self->base.base.base)->m_colorset);
  }
}
