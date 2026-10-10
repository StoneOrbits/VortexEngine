#ifndef CHASER_PATTERN_H
#define CHASER_PATTERN_H

#include "SequencedPattern.h"

// ChaserPattern is the demonstration sequenced pattern, it builds a sequence
// of steps where a single solid led chases around the map.
typedef struct ChaserPattern {
  // must remain the first member
  SequencedPattern base;
} ChaserPattern;

extern const PatternVtable ChaserPattern_vtable;

void ChaserPattern_init(ChaserPattern *self, const PatternArgs *args);

#endif
