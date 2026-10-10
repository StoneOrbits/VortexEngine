#ifndef DRIPMORPH_PATTERN_H
#define DRIPMORPH_PATTERN_H

#include <stdbool.h>
#include "MultiLedPattern.h"
#include "../../Time/Timer.h"
#include "../../Time/Timings.h"

typedef struct DripMorphPattern {
  // must remain the first member
  MultiLedPattern base;
  uint8_t m_blinkOnDuration;
  uint8_t m_blinkOffDuration;
  Timer m_blinkTimer;
  uint8_t m_speed;
  HSVColor m_cur;
  HSVColor m_next;
} DripMorphPattern;

// the vtable for this pattern
extern const PatternVtable DripMorphPattern_vtable;

// initialize a new DripMorphPattern
void DripMorphPattern_init(DripMorphPattern *self, const PatternArgs *args);
void DripMorphPattern_destroy(Pattern *self);

void DripMorphPattern_initImpl(Pattern *self);
void DripMorphPattern_play(Pattern *self);
void DripMorphPattern_blinkOn(DripMorphPattern *self);
void DripMorphPattern_blinkOff(DripMorphPattern *self);

#endif
