#include "CompoundPattern.h"

#include <string.h>

#include "../../Memory/Memory.h"
#include "../PatternBuilder.h"

const PatternVtable CompoundPattern_vtable = {
  .destroy = CompoundPattern_destroy,
  .bind = MultiLedPattern_bind,
  .init = CompoundPattern_initImpl,
  .play = CompoundPattern_play,
};

void CompoundPattern_init(CompoundPattern *self, const PatternArgs *args, const PatternVtable *vtable)
{
  memset(self->m_ledPatterns, 0, sizeof(self->m_ledPatterns));
  MultiLedPattern_init(&self->base, args, vtable);
}

// clean up all the sub patterns
void CompoundPattern_destroy(Pattern *pat)
{
  CompoundPattern *self = (CompoundPattern *)pat;
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (self->m_ledPatterns[pos]) {
      Pattern_destroy(self->m_ledPatterns[pos]);
      PatternBuilder_freePattern(self->m_ledPatterns[pos]);
      self->m_ledPatterns[pos] = NULL;
    }
  }
}

// init the pattern to initial state
void CompoundPattern_initImpl(Pattern *pat)
{
  CompoundPattern *self = (CompoundPattern *)pat;
  MultiLedPattern_doInit(pat);
}

// play the compound pattern (play all led patterns)
void CompoundPattern_play(Pattern *pat)
{
  CompoundPattern *self = (CompoundPattern *)pat;
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    SingleLedPattern *sub = self->m_ledPatterns[pos];
    if (!sub) {
      // should never happen, but just in case
      continue;
    }
    Pattern_play(sub);
  }
}

void CompoundPattern_clearPatterns(CompoundPattern *self)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (!self->m_ledPatterns[pos]) {
      continue;
    }
    Pattern_destroy(self->m_ledPatterns[pos]);
    PatternBuilder_freePattern(self->m_ledPatterns[pos]);
    self->m_ledPatterns[pos] = NULL;
  }
}

void CompoundPattern_setPatternAt(CompoundPattern *self, LedPos pos, SingleLedPattern *pat, const Colorset *set)
{
  if (!pat || pos >= LED_COUNT) {
    return;
  }
  Colorset use = set ? *set : self->base.base.m_colorset;
  Pattern_bind(pat, pos);
  Pattern_setColorset(pat, &use);
  Pattern_init(pat);
  // handle re-initialization and prevent leaks
  if (self->m_ledPatterns[pos]) {
    Pattern_destroy(self->m_ledPatterns[pos]);
    PatternBuilder_freePattern(self->m_ledPatterns[pos]);
  }
  // store the pattern for the given led
  self->m_ledPatterns[pos] = pat;
}

void CompoundPattern_setPatternAtId(CompoundPattern *self, LedPos pos, PatternID id, const PatternArgs *args, const Colorset *set)
{
  if (pos >= LED_COUNT) {
    return;
  }
  CompoundPattern_setPatternAt(self, pos, PatternBuilder_makeSingle(id, args), set);
}

void CompoundPattern_setEvensOdds(CompoundPattern *self, PatternID evenPattern, PatternID oddPattern,
  const PatternArgs *evenArgs, const PatternArgs *oddArgs)
{
  // Set the evenPattern on all evens and oddPattern on all odds
  for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
    const PatternArgs *args = isEven(p) ? evenArgs : oddArgs;
    PatternID id = isEven(p) ? evenPattern : oddPattern;
    CompoundPattern_setPatternAt(self, p, PatternBuilder_makeSingle(id, args), NULL);
  }
}
