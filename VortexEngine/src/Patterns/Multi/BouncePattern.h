#ifndef BOUNCE_PATTERN_H
#define BOUNCE_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct BouncePattern {
  // must remain the first member
  BlinkStepPattern base;
  uint8_t m_progress;
  uint8_t m_fadeAmount;
} BouncePattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable BouncePattern_bvtable;

// initialize a new BouncePattern
void BouncePattern_init(BouncePattern *self, const PatternArgs *args);
void BouncePattern_destroy(Pattern *self);

void BouncePattern_initImpl(Pattern *self);
void BouncePattern_blinkOn(BlinkStepPattern *self);
void BouncePattern_poststep(BlinkStepPattern *self);

#endif
