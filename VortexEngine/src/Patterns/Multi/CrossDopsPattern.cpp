#include "CrossDopsPattern.h"

#include <string.h>
#include "../../Leds/Leds.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable CrossDopsPattern_bvtable = {
  .base = {
    .destroy = CrossDopsPattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = CrossDopsPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = CrossDopsPattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = CrossDopsPattern_poststep,
};

void CrossDopsPattern_init(CrossDopsPattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &CrossDopsPattern_bvtable);
  self->m_ledMap = 0;
  Pattern_setPatternID((&self->base.base.base), PATTERN_CROSSDOPS);
  Pattern_setArgs((&self->base.base.base), args);
}

void CrossDopsPattern_destroy(Pattern *pat)
{
  CrossDopsPattern *self = (CrossDopsPattern *)pat;
  (void)self;
}

void CrossDopsPattern_initImpl(Pattern *pat)
{
  CrossDopsPattern *self = (CrossDopsPattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // Alternating evens and odds mapping of leds to turn on/off
  self->m_ledMap = MAP_PAIR_EVENS;
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 0);
}

void CrossDopsPattern_blinkOn(BlinkStepPattern *bsp)
{
  CrossDopsPattern *self = (CrossDopsPattern *)bsp;
  // set the current color on all the given leds
  Leds_setMap(self->m_ledMap, Colorset_cur(&(&self->base.base.base)->m_colorset));
}

void CrossDopsPattern_poststep(BlinkStepPattern *bsp)
{
  CrossDopsPattern *self = (CrossDopsPattern *)bsp;
  // iterate to next color and invert of all the evens/odds
  Colorset_getNext(&(&self->base.base.base)->m_colorset);
  self->m_ledMap = MAP_INVERSE(self->m_ledMap);
}
