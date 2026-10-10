#include "WarpPattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable WarpPattern_bvtable = {
  .base = {
    .destroy = WarpPattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = WarpPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = WarpPattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = WarpPattern_poststep,
};

void WarpPattern_init(WarpPattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &WarpPattern_bvtable);
  Pattern_setPatternID((&self->base.base.base), PATTERN_WARP);
  Pattern_setArgs((&self->base.base.base), args);
}

void WarpPattern_destroy(Pattern *pat)
{
  WarpPattern *self = (WarpPattern *)pat;
  (void)self;
}

void WarpPattern_initImpl(Pattern *pat)
{
  WarpPattern *self = (WarpPattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // reset progress
  self->m_progress = 0;
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 0);
}

void WarpPattern_blinkOn(BlinkStepPattern *bsp)
{
  WarpPattern *self = (WarpPattern *)bsp;
  Leds_setAll(Colorset_cur(&(&self->base.base.base)->m_colorset));
  Leds_setPair((Pair)self->m_progress, Colorset_peekNext(&(&self->base.base.base)->m_colorset));
}

void WarpPattern_poststep(BlinkStepPattern *bsp)
{
  WarpPattern *self = (WarpPattern *)bsp;
  self->m_progress = (self->m_progress + 1) % PAIR_COUNT;
  if (self->m_progress == 0) {
    Colorset_getNext(&(&self->base.base.base)->m_colorset);
  }
}
