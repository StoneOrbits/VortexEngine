#ifndef COMPOUND_PATTERN_H
#define COMPOUND_PATTERN_H

#include <stdbool.h>
#include "MultiLedPattern.h"
#include "../Single/SingleLedPattern.h"

// CompoundPattern is a multi-led pattern that displays pre-existing single
// led patterns simultaneously, one per led. It is abstract.
typedef struct CompoundPattern {
  // must remain the first member
  MultiLedPattern base;

  // array of single LED patterns, one for each LED
  SingleLedPattern *m_ledPatterns[LED_COUNT];
} CompoundPattern;

// CompoundPattern is abstract, so it supplies its own vtable rather than the
// plain MultiLedPattern one. Derived compound patterns reuse this table
// unless they override play().
extern const PatternVtable CompoundPattern_vtable;

// initialize the base portion of a CompoundPattern. No pattern id is
// assigned because CompoundPattern cannot be directly instantiated.
void CompoundPattern_init(CompoundPattern *self, const PatternArgs *args, const PatternVtable *vtable);

// vtable entry points, shared by every Compound derived pattern
void CompoundPattern_destroy(Pattern *self);
void CompoundPattern_initImpl(Pattern *self);
void CompoundPattern_play(Pattern *self);

// non-virtual helpers
void CompoundPattern_clearPatterns(CompoundPattern *self);
void CompoundPattern_setPatternAt(CompoundPattern *self, LedPos pos, SingleLedPattern *pat, const Colorset *set);
void CompoundPattern_setPatternAtId(CompoundPattern *self, LedPos pos, PatternID id, const PatternArgs *args, const Colorset *set);
void CompoundPattern_setEvensOdds(CompoundPattern *self, PatternID evenPattern, PatternID oddPattern,
  const PatternArgs *evenArgs, const PatternArgs *oddArgs);

#endif
