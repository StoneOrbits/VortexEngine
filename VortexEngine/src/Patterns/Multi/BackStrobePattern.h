#ifndef BACKSTROBE_PATTERN_H
#define BACKSTROBE_PATTERN_H

#include <stdbool.h>
#include "CompoundPattern.h"
#include <stdbool.h>
#include "../../Time/Timer.h"
#include "../../Time/Timings.h"

typedef struct BackStrobePattern {
  // must remain the first member
  CompoundPattern base;
  uint8_t m_stepSpeed;
  Timer m_stepTimer;
  bool m_switch;
  PatternArgs m_firstPatternArgs;
  PatternArgs m_secondPatternArgs;
  uint8_t m_firstPat;
  uint8_t m_secPat;
} BackStrobePattern;

// the vtable for this pattern
extern const PatternVtable BackStrobePattern_vtable;

// initialize a new BackStrobePattern
void BackStrobePattern_init(BackStrobePattern *self, const PatternArgs *args);
void BackStrobePattern_destroy(Pattern *self);

void BackStrobePattern_initImpl(Pattern *self);
void BackStrobePattern_play(Pattern *self);

#endif
