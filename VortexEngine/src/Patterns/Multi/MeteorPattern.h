#ifndef METEOR_PATTERN_H
#define METEOR_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"
#include "../../Leds/LedStash.h"
#include "../../Random/Random.h"

typedef struct MeteorPattern {
  // must remain the first member
  BlinkStepPattern base;
  uint8_t m_fadeAmount;
  Random m_randCtx;
  LedStash m_stash;
} MeteorPattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable MeteorPattern_bvtable;

// initialize a new MeteorPattern
void MeteorPattern_init(MeteorPattern *self, const PatternArgs *args);
void MeteorPattern_destroy(Pattern *self);

void MeteorPattern_blinkOn(BlinkStepPattern *self);
void MeteorPattern_blinkOff(BlinkStepPattern *self);
void MeteorPattern_poststep(BlinkStepPattern *self);

#endif
