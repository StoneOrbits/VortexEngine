#include "LighthousePattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable LighthousePattern_bvtable = {
  .base = {
    .destroy = LighthousePattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = LighthousePattern_initImpl,
    .play = LighthousePattern_play
  },
  .blinkOn = LighthousePattern_blinkOn,
  .blinkOff = LighthousePattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = LighthousePattern_poststep,
};

void LighthousePattern_init(LighthousePattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &LighthousePattern_bvtable);
  self->m_fadeAmount = 0;
  self->m_fadeRate = 0;
  Timer_make(&self->m_fadeTimer);
  LedStash_make(&self->m_stash);
  self->m_progress = 0;
  Pattern_setPatternID((&self->base.base.base), PATTERN_LIGHTHOUSE);
  REGISTER_ARG((&self->base.base.base), LighthousePattern, m_fadeAmount);
  REGISTER_ARG((&self->base.base.base), LighthousePattern, m_fadeRate);
  Pattern_setArgs((&self->base.base.base), args);
}

void LighthousePattern_destroy(Pattern *pat)
{
  LighthousePattern *self = (LighthousePattern *)pat;
  (void)self;
}

void LighthousePattern_initImpl(Pattern *pat)
{
  LighthousePattern *self = (LighthousePattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // fade timer setup
  Timer_reset(&self->m_fadeTimer);
  Timer_addAlarm(&self->m_fadeTimer, self->m_fadeRate);
  Timer_start0(&self->m_fadeTimer);
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 0);
}

void LighthousePattern_play(Pattern *pat)
{
  LighthousePattern *self = (LighthousePattern *)pat;
  if (Timer_alarm(&self->m_fadeTimer) == 0) {
    LighthousePattern_fade(self);
  }
  BlinkStepPattern_play(pat);
}

void LighthousePattern_blinkOn(BlinkStepPattern *bsp)
{
  LighthousePattern *self = (LighthousePattern *)bsp;
  Leds_restoreAll(&self->m_stash);
  Leds_setIndex((LedPos)self->m_progress, Colorset_cur(&(&self->base.base.base)->m_colorset));
  Leds_stashAll(&self->m_stash);
}

void LighthousePattern_blinkOff(BlinkStepPattern *bsp)
{
  LighthousePattern *self = (LighthousePattern *)bsp;
  Leds_stashAll(&self->m_stash);
  Leds_clearAll();
}

void LighthousePattern_poststep(BlinkStepPattern *bsp)
{
  LighthousePattern *self = (LighthousePattern *)bsp;
  self->m_progress = (self->m_progress + 1) % LED_COUNT;
  Colorset_getNext(&(&self->base.base.base)->m_colorset);
}

void LighthousePattern_fade(LighthousePattern *self)
{
  for (int i = 0; i < LED_COUNT; ++i) {
    RGBColor_adjustBrightness(LedStash_at(&self->m_stash, i), self->m_fadeAmount);
  }
}
