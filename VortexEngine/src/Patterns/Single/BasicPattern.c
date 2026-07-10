#include "BasicPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

const PatternVTable BasicPattern_vtable = {
  NULL,
  BasicPattern_play,
  BasicPattern_initVirtual,
  Pattern_bindBase,
  BasicPattern_onBlinkOn,
  BasicPattern_onBlinkOff,
  BasicPattern_beginGap,
  BasicPattern_beginDash
};

void BasicPattern_init(BasicPattern *self, const PatternArgs *args)
{
  SingleLedPattern_init(&self->base, args);
  self->base.base.vtable = &BasicPattern_vtable;
  self->onDuration = 0;
  self->offDuration = 0;
  self->gapDuration = 0;
  self->dashDuration = 0;
  self->groupSize = 0;
  self->groupCounter = 0;
  self->state = STATE_BLINK_ON;
  Timer_init(&self->blinkTimer);
  self->base.base.patternID = PATTERN_STROBE;
  REGISTER_ARG(self, self->onDuration);
  REGISTER_ARG(self, self->offDuration);
  REGISTER_ARG(self, self->gapDuration);
  REGISTER_ARG(self, self->dashDuration);
  REGISTER_ARG(self, self->groupSize);
  Pattern_setArgs(&self->base.base, args);
}

void BasicPattern_initVirtual(Pattern *base)
{
  BasicPattern *self = (BasicPattern *)base;

  Pattern_initBase(base);

  self->state = STATE_BLINK_ON;
  if (self->dashDuration > 0) {
    self->state = STATE_BEGIN_DASH;
  }
  if ((!self->onDuration && !self->dashDuration) || !Colorset_numColors(&base->colorset)) {
    self->state = STATE_DISABLED;
  }
  self->groupCounter = self->groupSize ? self->groupSize : (Colorset_numColors(&base->colorset) - (self->dashDuration != 0));
}

static void nextState(BasicPattern *self, uint8_t timing)
{
  Timer_initFlags(&self->blinkTimer, TIMER_1_ALARM | TIMER_START, timing, 0, 0, 0);
  self->state = (uint8_t)(self->state + 1);
}

void BasicPattern_play(Pattern *base)
{
  BasicPattern *self = (BasicPattern *)base;

replay:

  switch (self->state) {
  case STATE_DISABLED:
    return;
  case STATE_BLINK_ON:
    if (self->onDuration > 0) {
      base->vtable->onBlinkOn(base);
      --self->groupCounter;
      nextState(self, self->onDuration);
      return;
    }
    self->state = STATE_BLINK_OFF;
  case STATE_BLINK_OFF:
    if (self->groupCounter > 0 || (!self->gapDuration && !self->dashDuration)) {
      if (self->offDuration > 0) {
        base->vtable->onBlinkOff(base);
        nextState(self, self->offDuration);
        return;
      }
      if (self->groupCounter > 0 && self->onDuration > 0) {
        self->state = STATE_BLINK_ON;
        goto replay;
      }
    }
    self->state = STATE_BEGIN_GAP;
  case STATE_BEGIN_GAP:
    self->groupCounter = self->groupSize ? self->groupSize : (Colorset_numColors(&base->colorset) - (self->dashDuration != 0));
    if (self->gapDuration > 0) {
      base->vtable->beginGap(base);
      nextState(self, self->gapDuration);
      return;
    }
    self->state = STATE_BEGIN_DASH;
  case STATE_BEGIN_DASH:
    if (self->dashDuration > 0) {
      base->vtable->beginDash(base);
      nextState(self, self->dashDuration);
      return;
    }
    self->state = STATE_BEGIN_GAP2;
  case STATE_BEGIN_GAP2:
    if (self->dashDuration > 0 && self->gapDuration > 0) {
      base->vtable->beginGap(base);
      nextState(self, self->gapDuration);
      return;
    }
    self->state = STATE_BLINK_ON;
    goto replay;
  default:
    break;
  }

  if (Timer_alarm(&self->blinkTimer) == ALARM_NONE) {
    return;
  }

  if (self->state == STATE_IN_GAP2 || (self->state == STATE_OFF && self->groupCounter > 0)) {
    self->state = self->onDuration ? STATE_BLINK_ON : (self->dashDuration ? STATE_BEGIN_DASH : STATE_BEGIN_GAP);
  } else if (self->state == STATE_OFF && (!self->groupCounter || Colorset_numColors(&base->colorset) == 1)) {
    self->state = (self->groupCounter > 0) ? STATE_BLINK_ON : STATE_BEGIN_GAP;
  } else {
    self->state = (uint8_t)(self->state + 1);
  }
  goto replay;
}

void BasicPattern_onBlinkOn(Pattern *base)
{
  BasicPattern *self = (BasicPattern *)base;
  Leds_setIndex(base->ledPos, Colorset_getNext(&base->colorset));
  (void)self;
}

void BasicPattern_onBlinkOff(Pattern *base)
{
  BasicPattern *self = (BasicPattern *)base;
  Leds_clearIndex(base->ledPos);
  (void)self;
}

void BasicPattern_beginGap(Pattern *base)
{
  BasicPattern *self = (BasicPattern *)base;
  Leds_clearIndex(base->ledPos);
  (void)self;
}

void BasicPattern_beginDash(Pattern *base)
{
  BasicPattern *self = (BasicPattern *)base;
  Leds_setIndex(base->ledPos, Colorset_getNext(&base->colorset));
  (void)self;
}
