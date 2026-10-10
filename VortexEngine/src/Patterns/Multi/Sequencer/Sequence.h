#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <stdbool.h>
#include <stddef.h>
#include "../../Patterns.h"
#include "../../../Leds/LedTypes.h"
#include "../../../Colors/Colorset.h"
#include "../../../Time/Timer.h"

typedef struct ByteStream ByteStream;
typedef struct Sequence Sequence;
typedef struct SequenceStep SequenceStep;

// a map of leds to pattern ids
typedef struct PatternMap {
  // public list of pattern IDs for each led
  PatternID m_patternMap[LED_COUNT];
} PatternMap;

void PatternMap_make(PatternMap *self);
void PatternMap_makeWith(PatternMap *self, PatternID pattern, LedMap positions);
// set a pattern at each position in the LedMap
void PatternMap_setPatternAt(PatternMap *self, PatternID pattern, LedMap positions);
PatternID PatternMap_at(const PatternMap *self, LedPos index);
// serialize and unserialize a pattern map
bool PatternMap_serialize(const PatternMap *self, ByteStream *buffer);
bool PatternMap_unserialize(PatternMap *self, ByteStream *buffer);

// a map of leds to colorsets
typedef struct ColorsetMap {
  // public list of colorsets for each led
  Colorset m_colorsetMap[LED_COUNT];
} ColorsetMap;

void ColorsetMap_make(ColorsetMap *self);
void ColorsetMap_makeWith(ColorsetMap *self, const Colorset *colorset, LedMap positions);
// set a colorset at each position in the LedMap
void ColorsetMap_setColorsetAt(ColorsetMap *self, const Colorset *colorset, LedMap positions);
const Colorset *ColorsetMap_at(const ColorsetMap *self, LedPos index);
// serialize and unserialize a colorset map
bool ColorsetMap_serialize(const ColorsetMap *self, ByteStream *buffer);
bool ColorsetMap_unserialize(ColorsetMap *self, ByteStream *buffer);

// a single step in a sequence
struct SequenceStep {
  // public members to allow for easy initialization of an array of SequenceSteps
  uint16_t m_duration;
  PatternMap m_patternMap;
  ColorsetMap m_colorsetMap;
};

void SequenceStep_make(SequenceStep *self);
void SequenceStep_makeWith(SequenceStep *self, uint16_t duration, const PatternMap *patternMap, const ColorsetMap *colorsetMap);
// serialize and unserialize a step in the sequencer
bool SequenceStep_serialize(const SequenceStep *self, ByteStream *buffer);
bool SequenceStep_unserialize(SequenceStep *self, ByteStream *buffer);

// a sequence which can contain multiple SequenceSteps
struct Sequence {
  // static data
  SequenceStep *m_sequenceSteps;
  uint8_t m_numSteps;
};

void Sequence_make(Sequence *self);
void Sequence_destroy(Sequence *self);
void Sequence_copy(Sequence *self, const Sequence *other);
bool Sequence_equals(const Sequence *self, const Sequence *other);

void Sequence_initSteps(Sequence *self, uint8_t numSteps);
bool Sequence_addStep(Sequence *self, const SequenceStep *step);
bool Sequence_addStepWith(Sequence *self, uint16_t duration, const PatternMap *patternMap, const ColorsetMap *colorsetMap);
void Sequence_clear(Sequence *self);

bool Sequence_serialize(const Sequence *self, ByteStream *buffer);
bool Sequence_unserialize(Sequence *self, ByteStream *buffer);

static inline uint8_t Sequence_numSteps(const Sequence *self)
{
  return self->m_numSteps;
}

const SequenceStep *Sequence_at(const Sequence *self, uint8_t index);

#endif
