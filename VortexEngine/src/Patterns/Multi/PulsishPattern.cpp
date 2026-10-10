#include "PulsishPattern.h"

#include <string.h>
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const PatternVtable PulsishPattern_vtable = {
  .destroy = PulsishPattern_destroy,
  .bind = MultiLedPattern_bind,
  .init = PulsishPattern_initImpl,
  .play = PulsishPattern_play
};

void PulsishPattern_init(PulsishPattern *self, const PatternArgs *args)
{
  MultiLedPattern_init(&self->base, args, &PulsishPattern_vtable);
  self->m_progress = 0;
  self->m_stepDuration = 0;
  Timer_make(&self->m_stepTimer);
  self->m_onDuration1 = 0;
  self->m_offDuration1 = 0;
  self->m_onDuration2 = 0;
  self->m_offDuration2 = 0;
  Timer_make(&self->m_blinkTimer);
  Timer_make(&self->m_blink2Timer);
  Pattern_setPatternID((&self->base.base), PATTERN_PULSISH);
  REGISTER_ARG((&self->base.base), PulsishPattern, m_onDuration1);
  REGISTER_ARG((&self->base.base), PulsishPattern, m_offDuration1);
  REGISTER_ARG((&self->base.base), PulsishPattern, m_onDuration2);
  REGISTER_ARG((&self->base.base), PulsishPattern, m_offDuration2);
  REGISTER_ARG((&self->base.base), PulsishPattern, m_stepDuration);
  Pattern_setArgs((&self->base.base), args);
}

void PulsishPattern_destroy(Pattern *pat)
{
  PulsishPattern *self = (PulsishPattern *)pat;
  (void)self;
}

void PulsishPattern_initImpl(Pattern *pat)
{
  PulsishPattern *self = (PulsishPattern *)pat;
  MultiLedPattern_doInit(pat);
  // reset and add alarm
  Timer_reset(&self->m_stepTimer);
  Timer_addAlarm(&self->m_stepTimer, self->m_stepDuration);
  Timer_start0(&self->m_stepTimer);
  // make blink timer
  Timer_reset(&self->m_blinkTimer);
  Timer_addAlarm(&self->m_blinkTimer, self->m_onDuration1);
  Timer_addAlarm(&self->m_blinkTimer, self->m_offDuration1);
  Timer_start0(&self->m_blinkTimer);
  // make 2nd blink timer
  Timer_reset(&self->m_blink2Timer);
  Timer_addAlarm(&self->m_blink2Timer, self->m_onDuration2);
  Timer_addAlarm(&self->m_blink2Timer, self->m_offDuration2);
  Timer_start0(&self->m_blink2Timer);
}

void PulsishPattern_play(Pattern *pat)
{
  PulsishPattern *self = (PulsishPattern *)pat;
  // when the step timer triggers
  if (Timer_alarm(&self->m_stepTimer) == 0) {
    self->m_progress = (self->m_progress + 1) % PAIR_COUNT;
  }
  switch (Timer_alarm(&self->m_blinkTimer)) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    for (Pair pair = PAIR_FIRST; pair < PAIR_COUNT; ++pair) {
      if (pair != self->m_progress) {
        Leds_setPair(pair, Colorset_cur(&(&self->base.base)->m_colorset));
      }
    }
    Colorset_skip(&(&self->base.base)->m_colorset, 1);
    if (Colorset_curIndex(&(&self->base.base)->m_colorset) == 0) {
      Colorset_skip(&(&self->base.base)->m_colorset, 1);
    }
    break;
  case 1:
    for (Pair pair = PAIR_FIRST; pair < PAIR_COUNT; ++pair) {
      if (pair != self->m_progress) {
        Leds_clearPair(pair);
      }
    }
    break;
  }
  switch (Timer_alarm(&self->m_blink2Timer)) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    Leds_setPair((Pair)self->m_progress, Colorset_get(&(&self->base.base)->m_colorset, 0));
    break;
  case 1:
    Leds_clearPair((Pair)self->m_progress);
    break;
  }
}
