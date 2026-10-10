#include "TheaterChasePattern.h"

#include <string.h>
#include "../../Leds/Leds.h"

#define THEATER_CHASE_STEPS 10

#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable TheaterChasePattern_bvtable = {
  .base = {
    .destroy = TheaterChasePattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = TheaterChasePattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = TheaterChasePattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = TheaterChasePattern_poststep,
};

void TheaterChasePattern_init(TheaterChasePattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &TheaterChasePattern_bvtable);
  self->m_ledPositions = 0;
  Pattern_setPatternID((&self->base.base.base), PATTERN_THEATER_CHASE);
  Pattern_setArgs((&self->base.base.base), args);
}

void TheaterChasePattern_destroy(Pattern *pat)
{
  TheaterChasePattern *self = (TheaterChasePattern *)pat;
  (void)self;
}

void TheaterChasePattern_initImpl(Pattern *pat)
{
  TheaterChasePattern *self = (TheaterChasePattern *)pat;
  BlinkStepPattern_initImpl(pat);
  // starts on odd evens
  self->m_ledPositions = MAP_PAIR_ODD_EVENS;
  self->m_stepCounter = 0;
}

void TheaterChasePattern_blinkOn(BlinkStepPattern *bsp)
{
  TheaterChasePattern *self = (TheaterChasePattern *)bsp;
  Leds_setMap(self->m_ledPositions, Colorset_getNext(&(&self->base.base.base)->m_colorset));
}

void TheaterChasePattern_poststep(BlinkStepPattern *bsp)
{
  TheaterChasePattern *self = (TheaterChasePattern *)bsp;
  // the first 5 steps are odd evens/odds alternating each step
  if (self->m_stepCounter < 5) {
    self->m_ledPositions = (self->m_stepCounter % 2) ? MAP_PAIR_ODD_ODDS : MAP_PAIR_ODD_EVENS;
  } else {
    // the end 5 steps are even evens/odds alternating each step
    self->m_ledPositions = (self->m_stepCounter % 2) ? MAP_PAIR_EVEN_ODDS : MAP_PAIR_EVEN_EVENS;
  }
  // increment step counter
  self->m_stepCounter = (self->m_stepCounter + 1) % THEATER_CHASE_STEPS;
}
