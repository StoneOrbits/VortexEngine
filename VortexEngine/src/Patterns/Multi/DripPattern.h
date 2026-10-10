#ifndef DRIP_PATTERN_H
#define DRIP_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include <stdbool.h>
#include "../../Time/Timings.h"

typedef struct DripPattern {
  // must remain the first member
  BlinkStepPattern base;
  bool m_sync;
} DripPattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable DripPattern_bvtable;

// initialize a new DripPattern
void DripPattern_init(DripPattern *self, const PatternArgs *args);
void DripPattern_destroy(Pattern *self);

void DripPattern_initImpl(Pattern *self);
void DripPattern_blinkOn(BlinkStepPattern *self);
void DripPattern_poststep(BlinkStepPattern *self);

#endif
