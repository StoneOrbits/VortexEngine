#include "Sequence.h"

#include "../../../Serial/ByteStream.h"
#include "../../../Memory/Memory.h"
#include "../../../Leds/Leds.h"
#include "../../../Log/Log.h"

#include <string.h>

// some arbitrary number
#define MAX_SEQUENCE_STEPS 64

// ===================
//  PatternMap

void PatternMap_make(PatternMap *self)
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    self->m_patternMap[i] = PATTERN_NONE;
  }
}

void PatternMap_makeWith(PatternMap *self, PatternID pattern, LedMap positions)
{
  PatternMap_make(self);
  PatternMap_setPatternAt(self, pattern, positions);
}

// set a pattern at each position in the LedMap
void PatternMap_setPatternAt(PatternMap *self, PatternID pattern, LedMap positions)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (ledmapCheckLed(positions, pos)) {
      self->m_patternMap[pos] = pattern;
    }
  }
}

PatternID PatternMap_at(const PatternMap *self, LedPos index)
{
  return self->m_patternMap[index];
}

bool PatternMap_serialize(const PatternMap *self, ByteStream *buffer)
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    // ensure the PatternID is interpreted as uint8_t
    if (!ByteStream_serialize8(buffer, (uint8_t)self->m_patternMap[i])) {
      return false;
    }
  }
  return true;
}

bool PatternMap_unserialize(PatternMap *self, ByteStream *buffer)
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    if (!ByteStream_unserialize8(buffer, (uint8_t *)self->m_patternMap + i)) {
      return false;
    }
  }
  return true;
}

// ===================
//  ColorsetMap

void ColorsetMap_make(ColorsetMap *self)
{
  memset(self, 0, sizeof(*self));
}

void ColorsetMap_makeWith(ColorsetMap *self, const Colorset *colorset, LedMap positions)
{
  ColorsetMap_make(self);
  ColorsetMap_setColorsetAt(self, colorset, positions);
}

// set a colorset at each position in the LedMap
void ColorsetMap_setColorsetAt(ColorsetMap *self, const Colorset *colorset, LedMap positions)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (ledmapCheckLed(positions, pos)) {
      self->m_colorsetMap[pos] = *colorset;
    }
  }
}

const Colorset *ColorsetMap_at(const ColorsetMap *self, LedPos index)
{
  return &self->m_colorsetMap[index];
}

bool ColorsetMap_serialize(const ColorsetMap *self, ByteStream *buffer)
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    if (!Colorset_serialize(&self->m_colorsetMap[i], buffer)) {
      return false;
    }
  }
  return true;
}

bool ColorsetMap_unserialize(ColorsetMap *self, ByteStream *buffer)
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    if (!Colorset_unserialize(&self->m_colorsetMap[i], buffer)) {
      return false;
    }
  }
  return true;
}

// ===================
//  SequenceStep

void SequenceStep_make(SequenceStep *self)
{
  self->m_duration = 0;
  PatternMap_make(&self->m_patternMap);
  ColorsetMap_make(&self->m_colorsetMap);
}

void SequenceStep_makeWith(SequenceStep *self, uint16_t duration, const PatternMap *patternMap, const ColorsetMap *colorsetMap)
{
  self->m_duration = duration;
  self->m_patternMap = *patternMap;
  if (colorsetMap) {
    self->m_colorsetMap = *colorsetMap;
  } else {
    ColorsetMap_make(&self->m_colorsetMap);
  }
}

bool SequenceStep_serialize(const SequenceStep *self, ByteStream *buffer)
{
  if (!ByteStream_serialize16(buffer, self->m_duration)) {
    return false;
  }
  if (!PatternMap_serialize(&self->m_patternMap, buffer)) {
    return false;
  }
  if (!ColorsetMap_serialize(&self->m_colorsetMap, buffer)) {
    return false;
  }
  return true;
}

bool SequenceStep_unserialize(SequenceStep *self, ByteStream *buffer)
{
  if (!ByteStream_unserialize16(buffer, &self->m_duration)) {
    return false;
  }
  if (!PatternMap_unserialize(&self->m_patternMap, buffer)) {
    return false;
  }
  if (!ColorsetMap_unserialize(&self->m_colorsetMap, buffer)) {
    return false;
  }
  return true;
}

// ===================
//  Sequence

void Sequence_make(Sequence *self)
{
  self->m_sequenceSteps = NULL;
  self->m_numSteps = 0;
}

void Sequence_destroy(Sequence *self)
{
  Sequence_clear(self);
}

void Sequence_copy(Sequence *self, const Sequence *other)
{
  Sequence_clear(self);
  Sequence_initSteps(self, other->m_numSteps);
  for (uint8_t i = 0; i < other->m_numSteps; ++i) {
    self->m_sequenceSteps[i] = other->m_sequenceSteps[i];
  }
}

bool Sequence_equals(const Sequence *self, const Sequence *other)
{
  // only compare the palettes for equality
  return (self->m_numSteps == other->m_numSteps) &&
    (memcmp(self->m_sequenceSteps, other->m_sequenceSteps, self->m_numSteps * sizeof(SequenceStep)) == 0);
}

void Sequence_initSteps(Sequence *self, uint8_t numSteps)
{
  if (self->m_sequenceSteps) {
    vfree(self->m_sequenceSteps);
  }
  self->m_sequenceSteps = (SequenceStep *)vmalloc(sizeof(SequenceStep) * numSteps);
  if (!self->m_sequenceSteps) {
    ERROR_OUT_OF_MEMORY();
    return;
  }
  self->m_numSteps = numSteps;
}

bool Sequence_addStep(Sequence *self, const SequenceStep *step)
{
  if (self->m_numSteps >= MAX_SEQUENCE_STEPS) {
    return false;
  }
  // allocate a new palette one larger than before
  SequenceStep *temp = (SequenceStep *)vmalloc(sizeof(SequenceStep) * (self->m_numSteps + 1));
  if (!temp) {
    return false;
  }
  // if there is already some colors in the palette
  if (self->m_numSteps && self->m_sequenceSteps) {
    // copy over existing colors
    for (uint8_t i = 0; i < self->m_numSteps; ++i) {
      temp[i] = self->m_sequenceSteps[i];
    }
    // and free the existing palette
    vfree(self->m_sequenceSteps);
  }
  // reassign new palette
  self->m_sequenceSteps = temp;
  // insert new color and increment number of colors
  self->m_sequenceSteps[self->m_numSteps] = *step;
  self->m_numSteps++;
  return true;
}

bool Sequence_addStepWith(Sequence *self, uint16_t duration, const PatternMap *patternMap, const ColorsetMap *colorsetMap)
{
  SequenceStep step;
  SequenceStep_makeWith(&step, duration, patternMap, colorsetMap);
  bool ok = Sequence_addStep(self, &step);
  return ok;
}

void Sequence_clear(Sequence *self)
{
  if (self->m_sequenceSteps) {
    vfree(self->m_sequenceSteps);
    self->m_sequenceSteps = NULL;
  }
  self->m_numSteps = 0;
}

bool Sequence_serialize(const Sequence *self, ByteStream *buffer)
{
  if (!ByteStream_serialize8(buffer, self->m_numSteps)) {
    return false;
  }
  for (uint8_t i = 0; i < self->m_numSteps; ++i) {
    if (!SequenceStep_serialize(&self->m_sequenceSteps[i], buffer)) {
      return false;
    }
  }
  return true;
}

bool Sequence_unserialize(Sequence *self, ByteStream *buffer)
{
  if (!ByteStream_unserialize8(buffer, &self->m_numSteps)) {
    return false;
  }
  for (uint8_t i = 0; i < self->m_numSteps; ++i) {
    if (!SequenceStep_unserialize(&self->m_sequenceSteps[i], buffer)) {
      return false;
    }
  }
  return true;
}

const SequenceStep *Sequence_at(const Sequence *self, uint8_t index)
{
  return &self->m_sequenceSteps[index];
}
