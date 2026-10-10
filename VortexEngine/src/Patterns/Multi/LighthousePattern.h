#ifndef LIGHTHOUSE_PATTERN_H
#define LIGHTHOUSE_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Leds/LedStash.h"
#include "../../Time/Timings.h"
#include "../../Time/Timer.h"

typedef struct LighthousePattern {
  // must remain the first member
  BlinkStepPattern base;
  uint8_t m_fadeAmount;
  uint8_t m_fadeRate;
  Timer m_fadeTimer;
  LedStash m_stash;
  uint8_t m_progress;
} LighthousePattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable LighthousePattern_bvtable;

// initialize a new LighthousePattern
void LighthousePattern_init(LighthousePattern *self, const PatternArgs *args);
void LighthousePattern_destroy(Pattern *self);

void LighthousePattern_initImpl(Pattern *self);
void LighthousePattern_play(Pattern *self);
void LighthousePattern_blinkOn(BlinkStepPattern *self);
void LighthousePattern_blinkOff(BlinkStepPattern *self);
void LighthousePattern_poststep(BlinkStepPattern *self);
void LighthousePattern_fade(LighthousePattern *self);

#endif
