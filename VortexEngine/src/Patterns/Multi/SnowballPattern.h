#ifndef SNOWBALL_PATTERN_H
#define SNOWBALL_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct SnowballPattern {
  // must remain the first member
  BlinkStepPattern base;
  uint8_t m_progress;
} SnowballPattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable SnowballPattern_bvtable;

// initialize a new SnowballPattern
void SnowballPattern_init(SnowballPattern *self, const PatternArgs *args);
void SnowballPattern_destroy(Pattern *self);

void SnowballPattern_initImpl(Pattern *self);
void SnowballPattern_blinkOn(BlinkStepPattern *self);
void SnowballPattern_poststep(BlinkStepPattern *self);

#endif
