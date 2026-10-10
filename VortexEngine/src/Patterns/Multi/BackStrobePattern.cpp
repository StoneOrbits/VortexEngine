#include "BackStrobePattern.h"

#include <string.h>
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const PatternVtable BackStrobePattern_vtable = {
  .destroy = BackStrobePattern_destroy,
  .bind = MultiLedPattern_bind,
  .init = BackStrobePattern_initImpl,
  .play = BackStrobePattern_play
};

void BackStrobePattern_init(BackStrobePattern *self, const PatternArgs *args)
{
  CompoundPattern_init(&self->base, args, &BackStrobePattern_vtable);
  self->m_stepSpeed = 0;
  Timer_make(&self->m_stepTimer);
  self->m_switch = false;
  PatternArgs_make(&self->m_firstPatternArgs, 0, 0, 0);
  PatternArgs_make(&self->m_secondPatternArgs, 0, 0, 0);
  self->m_firstPat = PATTERN_DOPS;
  self->m_secPat = PATTERN_STROBE;
  Pattern_setPatternID((&self->base.base.base), PATTERN_BACKSTROBE);
  REGISTER_ARG((&self->base.base.base), BackStrobePattern, m_firstPatternArgs.arg1);
  REGISTER_ARG((&self->base.base.base), BackStrobePattern, m_firstPatternArgs.arg2);
  REGISTER_ARG((&self->base.base.base), BackStrobePattern, m_firstPatternArgs.arg3);
  REGISTER_ARG((&self->base.base.base), BackStrobePattern, m_secondPatternArgs.arg1);
  REGISTER_ARG((&self->base.base.base), BackStrobePattern, m_secondPatternArgs.arg2);
  REGISTER_ARG((&self->base.base.base), BackStrobePattern, m_stepSpeed);
  REGISTER_ARG((&self->base.base.base), BackStrobePattern, m_firstPat);
  REGISTER_ARG((&self->base.base.base), BackStrobePattern, m_secPat);
  Pattern_setArgs((&self->base.base.base), args);
}

void BackStrobePattern_destroy(Pattern *pat)
{
  // the compound base owns the sub patterns of each led, in c++ the base
  // destructor freed them automatically
  CompoundPattern_destroy(pat);
}

void BackStrobePattern_initImpl(Pattern *pat)
{
  BackStrobePattern *self = (BackStrobePattern *)pat;
  CompoundPattern_initImpl(pat);
  // timer for switch
  Timer_reset(&self->m_stepTimer);
  Timer_addAlarm(&self->m_stepTimer, self->m_stepSpeed * 100);
  Timer_start0(&self->m_stepTimer);
  // initialize the sub patterns one time first
  CompoundPattern_setEvensOdds(&self->base, (PatternID)self->m_firstPat, (PatternID)self->m_secPat, &self->m_firstPatternArgs, &self->m_secondPatternArgs);
}

void BackStrobePattern_play(Pattern *pat)
{
  BackStrobePattern *self = (BackStrobePattern *)pat;
  if (Timer_alarm(&self->m_stepTimer) == 0) {
    // switch which patterns are displayed
    self->m_switch = !self->m_switch;
    // update the tip/top patterns based on the switch
    CompoundPattern_setEvensOdds(&self->base, self->m_switch ? (PatternID)self->m_firstPat : (PatternID)self->m_secPat,
                self->m_switch ? (PatternID)self->m_secPat : (PatternID)self->m_firstPat,
                self->m_switch ? &self->m_firstPatternArgs : &self->m_secondPatternArgs,
                self->m_switch ? &self->m_secondPatternArgs : &self->m_firstPatternArgs);
  }
  CompoundPattern_play(pat);
}
