#ifndef SOLID_PATTERN_H
#define SOLID_PATTERN_H

#include "BasicPattern.h"

// SolidPattern is a BlinkPattern that always shows a single colour, chosen by
// the m_colIndex argument. It only replaces the onBlinkOn hook, the rest of
// the blinking state machine is reused unchanged.
typedef struct SolidPattern {
  // must remain the first member
  BlinkPattern base;

  // the index into the colorset to display, 0 means use the current one
  uint8_t m_colIndex;
} SolidPattern;

// the vtable for SolidPattern, identical to the blink one except that
// onBlinkOn is replaced
extern const BlinkPatternVtable SolidPattern_bvtable;

void SolidPattern_init(SolidPattern *self, PatternID id, const PatternArgs *args);
SolidPattern *SolidPattern_make(PatternID id, const PatternArgs *args);

// the replaced blinking hook
void SolidPattern_onBlinkOn(BlinkPattern *self);

#endif
