#ifndef WARP_PATTERN_H
#define WARP_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct WarpPattern {
  // must remain the first member
  BlinkStepPattern base;
  uint8_t m_progress;
} WarpPattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable WarpPattern_bvtable;

// initialize a new WarpPattern
void WarpPattern_init(WarpPattern *self, const PatternArgs *args);
void WarpPattern_destroy(Pattern *self);

void WarpPattern_initImpl(Pattern *self);
void WarpPattern_blinkOn(BlinkStepPattern *self);
void WarpPattern_poststep(BlinkStepPattern *self);

#endif
