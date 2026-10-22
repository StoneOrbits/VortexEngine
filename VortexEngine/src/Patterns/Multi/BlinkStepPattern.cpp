#include "BlinkStepPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include "../../Memory/Memory.h"

const BlinkStepVtable BlinkStepPattern_bvtable = {
  .base = {
    .destroy = Pattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = BlinkStepPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = BlinkStepPattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = BlinkStepPattern_poststep
};

void BlinkStepPattern_init(BlinkStepPattern *self, const PatternArgs *args, const BlinkStepVtable *vtable)
{
  self->bvtable = vtable;
  MultiLedPattern_init(&self->base, args, vtable ? &vtable->base : NULL);

  self->m_blinkOnDuration = 0;
  self->m_blinkOffDuration = 0;
  self->m_stepDuration = 0;
  Timer_make(&self->m_blinkTimer);
  Timer_make(&self->m_stepTimer);

  Pattern_setPatternID(&self->base.base, PATTERN_NONE);
  Pattern_setFlags(&self->base.base, Pattern_getFlags(&self->base.base) | PATTERN_FLAG_MULTI);

  REGISTER_ARG((Pattern *)&self->base.base, BlinkStepPattern, m_blinkOnDuration);
  REGISTER_ARG((Pattern *)&self->base.base, BlinkStepPattern, m_blinkOffDuration);
  REGISTER_ARG((Pattern *)&self->base.base, BlinkStepPattern, m_stepDuration);

  Pattern_setArgs(&self->base.base, args);
}

// init the pattern to initial state
void BlinkStepPattern_initImpl(Pattern *pat)
{
  BlinkStepPattern *self = (BlinkStepPattern *)pat;

  MultiLedPattern_doInit(pat);

  // reset the blink timer entirely
  Timer_reset(&self->m_blinkTimer);
  // dops timing
  Timer_addAlarm1(&self->m_blinkTimer, self->m_blinkOnDuration);
  Timer_addAlarm1(&self->m_blinkTimer, self->m_blinkOffDuration);
  // start the blink timer from the next frame
  Timer_start0(&self->m_blinkTimer);

  // reset and add alarm
  Timer_reset(&self->m_stepTimer);
  Timer_addAlarm1(&self->m_stepTimer, self->m_stepDuration);
  Timer_start0(&self->m_stepTimer);
}

// pure virtual must override the play function
void BlinkStepPattern_play(Pattern *pat)
{
  BlinkStepPattern *self = (BlinkStepPattern *)pat;

  // whether to trigger a step event based on the step timer
  bool shouldStep = (Timer_alarm(&self->m_stepTimer) == 0);
  if (shouldStep) {
    self->bvtable->prestep(self);
  }
  switch (Timer_alarm(&self->m_blinkTimer)) {
  case -1: // just break and still run post-step
    break;
  case 0: // turn on the leds for given mapping
    self->bvtable->blinkOn(self);
    break;
  case 1: // turn off the leds
    self->bvtable->blinkOff(self);
    break;
  }
  if (shouldStep) {
    self->bvtable->poststep(self);
  }
}

void BlinkStepPattern_blinkOn(BlinkStepPattern *self)
{
  // override me
}

void BlinkStepPattern_blinkOff(BlinkStepPattern *self)
{
  // override me
  Leds_clearAll();
}

void BlinkStepPattern_prestep(BlinkStepPattern *self)
{
  // override me
}

void BlinkStepPattern_poststep(BlinkStepPattern *self)
{
  // override me
  Colorset_getNext(&self->base.base.m_colorset);
}
