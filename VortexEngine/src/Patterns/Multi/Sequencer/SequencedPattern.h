#ifndef SEQUENCED_PATTERN_H
#define SEQUENCED_PATTERN_H

#include <stdbool.h>
#include "../CompoundPattern.h"

#include "../../../Leds/LedTypes.h"
#include "../../../Time/Timer.h"

#include "Sequence.h"

// SequencedPattern is abstract, it drives a CompoundPattern through an
// ordered list of steps, each of which assigns a pattern to every led.
typedef struct SequencedPattern {
  // must remain the first member
  CompoundPattern base;

  // static data
  Sequence m_sequence;

  // runtime data
  uint8_t m_curSequence;
  Timer m_timer;
} SequencedPattern;

// the vtable shared by every SequencedPattern
extern const PatternVtable SequencedPattern_vtable;

void SequencedPattern_init(SequencedPattern *self, const PatternArgs *args, const PatternVtable *vtable);
void SequencedPattern_initWithSequence(SequencedPattern *self, const PatternArgs *args, const Sequence *sequence, const PatternVtable *vtable);

void SequencedPattern_destroy(Pattern *self);
void SequencedPattern_initImpl(Pattern *self);
void SequencedPattern_play(Pattern *self);

// change the sequence of the pattern
void SequencedPattern_bindSequence(SequencedPattern *self, const Sequence *sequence);

#endif
