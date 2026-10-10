#ifndef HUE_SHIFT_PATTERN_H
#define HUE_SHIFT_PATTERN_H

#include <stdbool.h>
#include "MultiLedPattern.h"
#include "../../Time/Timings.h"
#include "../../Time/Timer.h"

typedef struct HueShiftPattern {
  // must remain the first member
  MultiLedPattern base;
  uint8_t m_blinkOnDuration;
  uint8_t m_blinkOffDuration;
  uint8_t m_blendDelay;
  uint8_t m_delayCounter;
  Timer m_blinkTimer;
  HSVColor m_cur;
  HSVColor m_next;
} HueShiftPattern;

// the vtable for this pattern
extern const PatternVtable HueShiftPattern_vtable;

// initialize a new HueShiftPattern
void HueShiftPattern_init(HueShiftPattern *self, const PatternArgs *args);
void HueShiftPattern_destroy(Pattern *self);

void HueShiftPattern_initImpl(Pattern *self);
void HueShiftPattern_play(Pattern *self);

#endif
