#include "VortexWipePattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable VortexWipePattern_bvtable = {
  .base = {
    .destroy = VortexWipePattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = VortexWipePattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = VortexWipePattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = VortexWipePattern_poststep,
};

void VortexWipePattern_init(VortexWipePattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &VortexWipePattern_bvtable);
  Pattern_setPatternID((&self->base.base.base), PATTERN_VORTEXWIPE);
  Pattern_setArgs((&self->base.base.base), args);
}

const LedPos VortexWipePattern_ledStepPositions[] = {
  LED_9,
  LED_7,
  LED_5,
  LED_3,
  LED_1,

  LED_0,
  LED_2,
  LED_4,
  LED_6,
  LED_8
};

void VortexWipePattern_destroy(Pattern *pat)
{
  VortexWipePattern *self = (VortexWipePattern *)pat;
  (void)self;
}

void VortexWipePattern_initImpl(Pattern *pat)
{
  VortexWipePattern *self = (VortexWipePattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // reset progress
  self->m_progress = 0;
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 0);
}

void VortexWipePattern_blinkOn(BlinkStepPattern *bsp)
{
  VortexWipePattern *self = (VortexWipePattern *)bsp;
  for (int index = 0; index < self->m_progress; ++index) {
    Leds_setIndex(VortexWipePattern_ledStepPositions[index], Colorset_peekNext(&(&self->base.base.base)->m_colorset));
  }
  for (int index = self->m_progress; index < LED_COUNT; ++index) {
    Leds_setIndex(VortexWipePattern_ledStepPositions[index], Colorset_cur(&(&self->base.base.base)->m_colorset));
  }
}

void VortexWipePattern_poststep(BlinkStepPattern *bsp)
{
  VortexWipePattern *self = (VortexWipePattern *)bsp;
  self->m_progress = (self->m_progress + 1) % LED_COUNT;
  if (self->m_progress == 0) {
    Colorset_getNext(&(&self->base.base.base)->m_colorset);
  }
}
