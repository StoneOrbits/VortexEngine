#include "BouncePattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#define TOTAL_STEPS (((PAIR_COUNT * 2) - 2) ? ((PAIR_COUNT * 2) - 2) : 1)
#define HALF_STEPS (TOTAL_STEPS / 2)

#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable BouncePattern_bvtable = {
  .base = {
    .destroy = BouncePattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = BouncePattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = BouncePattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = BouncePattern_poststep,
};

void BouncePattern_init(BouncePattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &BouncePattern_bvtable);
  self->m_fadeAmount = 0;
  Pattern_setPatternID((&self->base.base.base), PATTERN_BOUNCE);
  REGISTER_ARG((&self->base.base.base), BouncePattern, m_fadeAmount);
  Pattern_setArgs((&self->base.base.base), args);
}

void BouncePattern_destroy(Pattern *pat)
{
  BouncePattern *self = (BouncePattern *)pat;
  (void)self;
}

void BouncePattern_initImpl(Pattern *pat)
{
  BouncePattern *self = (BouncePattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // reset progress
  self->m_progress = 0;
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 0);
}

void BouncePattern_blinkOn(BlinkStepPattern *bsp)
{
  BouncePattern *self = (BouncePattern *)bsp;
  Leds_setAll(Colorset_cur(&(&self->base.base.base)->m_colorset));
  if (self->m_progress < PAIR_COUNT) {
    Leds_setPair((Pair)self->m_progress, Colorset_peekNext(&(&self->base.base.base)->m_colorset));
  } else {
    Leds_setPair((Pair)(TOTAL_STEPS - self->m_progress), Colorset_peekNext(&(&self->base.base.base)->m_colorset));
  }
}

void BouncePattern_poststep(BlinkStepPattern *bsp)
{
  BouncePattern *self = (BouncePattern *)bsp;
  self->m_progress = (self->m_progress + 1) % TOTAL_STEPS;
  if (self->m_progress == 0 || self->m_progress == HALF_STEPS) {
    Colorset_getNext(&(&self->base.base.base)->m_colorset);
  }
}
