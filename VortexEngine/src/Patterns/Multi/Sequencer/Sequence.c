#include "../../Patterns/Multi/Sequencer/Sequence.h"

#include <string.h>

#if VORTEX_SLIM == 1

void PatternMap_init(PatternMap *self) {
  memset(self, 0, sizeof(*self));
}

void PatternMap_initWithMap(PatternMap *self, PatternID pattern, LedMap positions) {
  PatternMap_init(self);
  PatternMap_setPatternAt(self, pattern, positions);
}

void PatternMap_setPatternAt(PatternMap *self, PatternID pattern, LedMap positions) {
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (ledmapCheckLed(positions, pos)) {
      self->m_patternMap[pos] = pattern;
    }
  }
}

PatternID PatternMap_get(const PatternMap *self, LedPos index) {
  return self->m_patternMap[index];
}

void ColorsetMap_init(ColorsetMap *self) {
  memset(self, 0, sizeof(*self));
}

void ColorsetMap_initWithMap(ColorsetMap *self, const Colorset *colorset, LedMap positions) {
  ColorsetMap_init(self);
  ColorsetMap_setColorsetAt(self, colorset, positions);
}

void ColorsetMap_setColorsetAt(ColorsetMap *self, const Colorset *colorset, LedMap positions) {
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (ledmapCheckLed(positions, pos)) {
      self->m_colorsetMap[pos] = *colorset;
    }
  }
}

const Colorset *ColorsetMap_get(const ColorsetMap *self, LedPos index) {
  return &self->m_colorsetMap[index];
}

void SequenceStep_init(SequenceStep *self) {
  self->m_duration = 0;
  PatternMap_init(&self->m_patternMap);
  ColorsetMap_init(&self->m_colorsetMap);
}

void SequenceStep_initFull(SequenceStep *self, uint16_t duration, const PatternMap *patternMap, const ColorsetMap *colorsetMap) {
  self->m_duration = duration;
  self->m_patternMap = *patternMap;
  self->m_colorsetMap = *colorsetMap;
}

void Sequence_init(Sequence *self) {
  self->m_sequenceSteps = NULL;
  self->m_numSteps = 0;
}

void Sequence_cleanup(Sequence *self) {
  Sequence_clear(self);
}

uint8_t Sequence_numSteps(const Sequence *self) {
  return self->m_numSteps;
}

void Sequence_clear(Sequence *self) {
  self->m_sequenceSteps = NULL;
  self->m_numSteps = 0;
}

#endif
