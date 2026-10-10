#ifndef CROSSDOPS_PATTERN_H
#define CROSSDOPS_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct CrossDopsPattern {
  // must remain the first member
  BlinkStepPattern base;
  LedMap m_ledMap;
} CrossDopsPattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable CrossDopsPattern_bvtable;

// initialize a new CrossDopsPattern
void CrossDopsPattern_init(CrossDopsPattern *self, const PatternArgs *args);
void CrossDopsPattern_destroy(Pattern *self);

void CrossDopsPattern_initImpl(Pattern *self);
void CrossDopsPattern_blinkOn(BlinkStepPattern *self);
void CrossDopsPattern_poststep(BlinkStepPattern *self);

#endif
