#include "WarpWormPattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable WarpWormPattern_bvtable = {
  .base = {
    .destroy = WarpWormPattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = WarpWormPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = WarpWormPattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = WarpWormPattern_poststep,
};

void WarpWormPattern_init(WarpWormPattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &WarpWormPattern_bvtable);
  Pattern_setPatternID((&self->base.base.base), PATTERN_WARPWORM);
  Pattern_setArgs((&self->base.base.base), args);
}

void WarpWormPattern_destroy(Pattern *pat)
{
  WarpWormPattern *self = (WarpWormPattern *)pat;
  (void)self;
}

void WarpWormPattern_initImpl(Pattern *pat)
{
  WarpWormPattern *self = (WarpWormPattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // reset progress
  self->m_progress = 0;
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 1);
}

void WarpWormPattern_blinkOn(BlinkStepPattern *bsp)
{
  WarpWormPattern *self = (WarpWormPattern *)bsp;
  int wormSize = 6;
  Leds_setAll(Colorset_get(&(&self->base.base.base)->m_colorset, 0));
  for (int body = 0; body < wormSize; ++body) {
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

void WarpWormPattern_poststep(BlinkStepPattern *bsp)
{
  WarpWormPattern *self = (WarpWormPattern *)bsp;
  self->m_progress = (self->m_progress + 1) % LED_COUNT;
  if (self->m_progress == 0) {
    Colorset_getNext(&(&self->base.base.base)->m_colorset);
    if (Colorset_curIndex(&(&self->base.base.base)->m_colorset) == 0) {
      Colorset_getNext(&(&self->base.base.base)->m_colorset);
    }
  }
}
