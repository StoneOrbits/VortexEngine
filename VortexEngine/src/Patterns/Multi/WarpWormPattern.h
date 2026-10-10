#ifndef WARP_WORM_PATTERN_H
#define WARP_WORM_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct WarpWormPattern {
  // must remain the first member
  BlinkStepPattern base;
  uint8_t m_progress;
} WarpWormPattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable WarpWormPattern_bvtable;

// initialize a new WarpWormPattern
void WarpWormPattern_init(WarpWormPattern *self, const PatternArgs *args);
void WarpWormPattern_destroy(Pattern *self);

void WarpWormPattern_initImpl(Pattern *self);
void WarpWormPattern_blinkOn(BlinkStepPattern *self);
void WarpWormPattern_poststep(BlinkStepPattern *self);

#endif
