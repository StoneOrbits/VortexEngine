#ifndef FILL_PATTERN_H
#define FILL_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct FillPattern {
  // must remain the first member
  BlinkStepPattern base;
  uint8_t m_progress;
} FillPattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable FillPattern_bvtable;

// initialize a new FillPattern
void FillPattern_init(FillPattern *self, const PatternArgs *args);
void FillPattern_destroy(Pattern *self);

void FillPattern_initImpl(Pattern *self);
void FillPattern_blinkOn(BlinkStepPattern *self);
void FillPattern_poststep(BlinkStepPattern *self);

#endif
