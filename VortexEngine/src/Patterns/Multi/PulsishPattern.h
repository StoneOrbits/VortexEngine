#ifndef PULSISH_PATTERN_H
#define PULSISH_PATTERN_H

#include <stdbool.h>
#include "MultiLedPattern.h"
#include "../../Time/Timings.h"
#include "../../Time/Timer.h"

typedef struct PulsishPattern {
  // must remain the first member
  MultiLedPattern base;
  uint8_t m_progress;
  uint8_t m_stepDuration;
  Timer m_stepTimer;
  uint8_t m_onDuration1;
  uint8_t m_offDuration1;
  uint8_t m_onDuration2;
  uint8_t m_offDuration2;
  Timer m_blinkTimer;
  Timer m_blink2Timer;
} PulsishPattern;

// the vtable for this pattern
extern const PatternVtable PulsishPattern_vtable;

// initialize a new PulsishPattern
void PulsishPattern_init(PulsishPattern *self, const PatternArgs *args);
void PulsishPattern_destroy(Pattern *self);

void PulsishPattern_initImpl(Pattern *self);
void PulsishPattern_play(Pattern *self);

#endif
