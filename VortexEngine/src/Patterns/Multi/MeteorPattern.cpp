#include "MeteorPattern.h"

#include <string.h>
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable MeteorPattern_bvtable = {
  .base = {
    .destroy = MeteorPattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = BlinkStepPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = MeteorPattern_blinkOn,
  .blinkOff = MeteorPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = MeteorPattern_poststep,
};

void MeteorPattern_init(MeteorPattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &MeteorPattern_bvtable);
  self->m_fadeAmount = 0;
  Random_make(&self->m_randCtx);
  LedStash_make(&self->m_stash);
  Pattern_setPatternID((&self->base.base.base), PATTERN_METEOR);
  REGISTER_ARG((&self->base.base.base), MeteorPattern, m_fadeAmount);
  Pattern_setArgs((&self->base.base.base), args);
}

void MeteorPattern_destroy(Pattern *pat)
{
  MeteorPattern *self = (MeteorPattern *)pat;
  (void)self;
}

void MeteorPattern_blinkOn(BlinkStepPattern *bsp)
{
  MeteorPattern *self = (MeteorPattern *)bsp;
  for (int i = 0; i < LED_COUNT; ++i) {
    RGBColor_adjustBrightness(LedStash_at(&self->m_stash, i), self->m_fadeAmount);
  }
  Leds_restoreAll(&self->m_stash);
}

void MeteorPattern_blinkOff(BlinkStepPattern *bsp)
{
  MeteorPattern *self = (MeteorPattern *)bsp;
  Leds_clearAll();
}

void MeteorPattern_poststep(BlinkStepPattern *bsp)
{
  MeteorPattern *self = (MeteorPattern *)bsp;
  for (uint8_t meteorCount = 0; meteorCount < (LED_COUNT / 2); ++meteorCount) {
    // when a new meteor is created it is incerted into the stash so the blinking pattern is not interrupted
    LedPos target = (LedPos)Random_next8(&self->m_randCtx, LED_FIRST, LED_LAST);
    RGBColor col = Colorset_getNext(&(&self->base.base.base)->m_colorset);
    LedStash_setIndex(&self->m_stash, target, col);
  }
}
