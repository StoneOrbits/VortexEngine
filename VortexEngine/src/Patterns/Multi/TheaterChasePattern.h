#ifndef THEATER_CHASE_PATTERN_H
#define THEATER_CHASE_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Time/Timings.h"

typedef struct TheaterChasePattern {
  // must remain the first member
  BlinkStepPattern base;
  LedMap m_ledPositions;
  uint8_t m_stepCounter;
} TheaterChasePattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable TheaterChasePattern_bvtable;

// initialize a new TheaterChasePattern
void TheaterChasePattern_init(TheaterChasePattern *self, const PatternArgs *args);
void TheaterChasePattern_destroy(Pattern *self);

void TheaterChasePattern_initImpl(Pattern *self);
void TheaterChasePattern_blinkOn(BlinkStepPattern *self);
void TheaterChasePattern_poststep(BlinkStepPattern *self);

#endif
