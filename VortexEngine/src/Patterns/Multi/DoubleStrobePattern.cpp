#include "DoubleStrobePattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Leds/Leds.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable DoubleStrobePattern_bvtable = {
  .base = {
    .destroy = DoubleStrobePattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = DoubleStrobePattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = DoubleStrobePattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = BlinkStepPattern_poststep,
};

void DoubleStrobePattern_init(DoubleStrobePattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &DoubleStrobePattern_bvtable);
  Pattern_setPatternID((&self->base.base.base), PATTERN_DOUBLESTROBE);
  Pattern_setArgs((&self->base.base.base), args);
}

void DoubleStrobePattern_destroy(Pattern *pat)
{
  DoubleStrobePattern *self = (DoubleStrobePattern *)pat;
  (void)self;
}

void DoubleStrobePattern_initImpl(Pattern *pat)
{
  DoubleStrobePattern *self = (DoubleStrobePattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 0);
}

void DoubleStrobePattern_blinkOn(BlinkStepPattern *bsp)
{
  DoubleStrobePattern *self = (DoubleStrobePattern *)bsp;
  Leds_setAllOdds(Colorset_cur(&(&self->base.base.base)->m_colorset));
  Leds_setAllEvens(Colorset_peekNext(&(&self->base.base.base)->m_colorset));
}
