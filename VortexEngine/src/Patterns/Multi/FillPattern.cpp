#include "FillPattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable FillPattern_bvtable = {
  .base = {
    .destroy = FillPattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = FillPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = FillPattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = FillPattern_poststep,
};

void FillPattern_init(FillPattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &FillPattern_bvtable);
  Pattern_setPatternID((&self->base.base.base), PATTERN_FILL);
  Pattern_setArgs((&self->base.base.base), args);
}

void FillPattern_destroy(Pattern *pat)
{
  FillPattern *self = (FillPattern *)pat;
  (void)self;
}

void FillPattern_initImpl(Pattern *pat)
{
  FillPattern *self = (FillPattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // reset progress
  self->m_progress = 0;
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 0);
}

void FillPattern_blinkOn(BlinkStepPattern *bsp)
{
  FillPattern *self = (FillPattern *)bsp;
  Leds_setPairs(PAIR_FIRST, (Pair)self->m_progress, Colorset_peekNext(&(&self->base.base.base)->m_colorset));
  Leds_setPairs((Pair)self->m_progress, PAIR_COUNT, Colorset_cur(&(&self->base.base.base)->m_colorset));
}

void FillPattern_poststep(BlinkStepPattern *bsp)
{
  FillPattern *self = (FillPattern *)bsp;
  self->m_progress = (self->m_progress + 1) % PAIR_COUNT;
  if (self->m_progress == 0) {
    Colorset_getNext(&(&self->base.base.base)->m_colorset);
  }
}
