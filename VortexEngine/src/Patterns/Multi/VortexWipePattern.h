#ifndef VORTEXWIPE_PATTERN_H
#define VORTEXWIPE_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct VortexWipePattern {
  // must remain the first member
  BlinkStepPattern base;
  static const LedPos ledStepPositions;
  uint8_t m_progress;
} VortexWipePattern;

extern const LedPos VortexWipePattern_ledStepPositions[];

// the blink/step vtable for this pattern
extern const BlinkStepVtable VortexWipePattern_bvtable;

// initialize a new VortexWipePattern
void VortexWipePattern_init(VortexWipePattern *self, const PatternArgs *args);
void VortexWipePattern_destroy(Pattern *self);

void VortexWipePattern_initImpl(Pattern *self);
void VortexWipePattern_blinkOn(BlinkStepPattern *self);
void VortexWipePattern_poststep(BlinkStepPattern *self);

#endif
