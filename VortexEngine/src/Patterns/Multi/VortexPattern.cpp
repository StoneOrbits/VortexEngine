#include "VortexPattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#define MIDDLE_POINT ((LED_COUNT + 1) / 2)

#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable VortexPattern_bvtable = {
  .base = {
    .destroy = VortexPattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = VortexPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = VortexPattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = VortexPattern_poststep,
};

void VortexPattern_init(VortexPattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &VortexPattern_bvtable);

}

void VortexPattern_destroy(Pattern *pat)
{
  VortexPattern *self = (VortexPattern *)pat;
  (void)self;
}

void VortexPattern_initImpl(Pattern *pat)
{
  VortexPattern *self = (VortexPattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // reset progress
  self->m_progress = 0;
  // start colorset at index 0 so cur() works
  Colorset_setCurIndex(&(&self->base.base.base)->m_colorset, 0);
}

void VortexPattern_blinkOn(BlinkStepPattern *bsp)
{
  VortexPattern *self = (VortexPattern *)bsp;
  // Sets an LED at opposite ends of the strip and progresses towards the center
  Leds_setIndex((LedPos)self->m_progress, Colorset_peekNext(&(&self->base.base.base)->m_colorset));
  Leds_setIndex((LedPos)(LED_LAST - self->m_progress), Colorset_peekNext(&(&self->base.base.base)->m_colorset));
}

void VortexPattern_poststep(BlinkStepPattern *bsp)
{
  VortexPattern *self = (VortexPattern *)bsp;
  // step till the middle point
  self->m_progress = (self->m_progress + 1) % MIDDLE_POINT;
  // each cycle progress to the next color
  if (self->m_progress == 0) {
    Colorset_getNext(&(&self->base.base.base)->m_colorset);
  }
}
