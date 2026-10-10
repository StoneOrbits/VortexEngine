#ifndef DOUBLESTROBE_PATTERN_H
#define DOUBLESTROBE_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct DoubleStrobePattern {
  // must remain the first member
  BlinkStepPattern base;
} DoubleStrobePattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable DoubleStrobePattern_bvtable;

// initialize a new DoubleStrobePattern
void DoubleStrobePattern_init(DoubleStrobePattern *self, const PatternArgs *args);
void DoubleStrobePattern_destroy(Pattern *self);

void DoubleStrobePattern_initImpl(Pattern *self);
void DoubleStrobePattern_blinkOn(BlinkStepPattern *self);

#endif
