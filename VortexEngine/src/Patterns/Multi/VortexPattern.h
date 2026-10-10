#ifndef VORTEX_PATTERN_H
#define VORTEX_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct VortexPattern {
  // must remain the first member
  BlinkStepPattern base;
  uint8_t m_progress;
} VortexPattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable VortexPattern_bvtable;

// initialize a new VortexPattern
void VortexPattern_init(VortexPattern *self, const PatternArgs *args);
void VortexPattern_destroy(Pattern *self);

void VortexPattern_initImpl(Pattern *self);
void VortexPattern_blinkOn(BlinkStepPattern *self);
void VortexPattern_poststep(BlinkStepPattern *self);

#endif
