#include "Sequence.h"

#include "../../../VortexEngine.h"

#include "../../../Serial/ByteStream.h"
#include "../../../Memory/Memory.h"
#include "../../../Leds/Leds.h"
#include "../../../Log/Log.h"

#include <string.h>

// some arbitrary number
#define MAX_SEQUENCE_STEPS 64

PatternMap::PatternMap(VortexEngine &engine) :
  m_engine(engine),
  m_patternMap()
{
#if FIXED_LED_COUNT == 0
  m_patternMap.resize(LED_COUNT);
#endif
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    m_patternMap[i] = PATTERN_NONE;
  }
}

PatternMap::PatternMap(VortexEngine &engine, PatternID pattern, LedMap positions) :
  PatternMap(engine)
{
  setPatternAt(pattern, positions);
}

// set a pattern at each position in the LedMap
void PatternMap::setPatternAt(PatternID pattern, LedMap positions)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (m_engine.leds().mapCheckLed(positions, pos)) {
      m_patternMap[pos] = pattern;
    }
  }
}

PatternID PatternMap::operator[](LedPos index) const
{
  return m_patternMap[index];
}

void PatternMap::serialize(ByteStream &buffer) const
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    // ensure the PatternID is interpreted as uint8_t
    buffer.serialize((uint8_t)m_patternMap[i]);
  }
}

void PatternMap::unserialize(ByteStream &buffer)
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
#if FIXED_LED_COUNT == 0
    buffer.unserialize((uint8_t *)m_patternMap.data() + i);
#else
    buffer.unserialize((uint8_t *)m_patternMap + i);
#endif
  }
}

ColorsetMap::ColorsetMap(VortexEngine &engine) :
  m_engine(engine),
  m_colorsetMap()
{
}

ColorsetMap::ColorsetMap(VortexEngine &engine, const Colorset &colorset, LedMap positions) :
  ColorsetMap(engine)
{
  setColorsetAt(colorset, positions);
}

// set a pattern at each position in the LedMap
void ColorsetMap::setColorsetAt(const Colorset &colorset, LedMap positions)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (m_engine.leds().mapCheckLed(positions, pos)) {
      m_colorsetMap[pos] = colorset;
    }
  }
}
const Colorset &ColorsetMap::operator[](LedPos index) const
{
  return m_colorsetMap[index];
}

void ColorsetMap::serialize(ByteStream &buffer) const
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    m_colorsetMap[i].serialize(buffer);
  }
}

void ColorsetMap::unserialize(ByteStream &buffer)
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    m_colorsetMap[i].unserialize(buffer);
  }
}

// Make an array of sequence steps to create a sequenced pattern
SequenceStep::SequenceStep(VortexEngine &engine) :
  m_engine(engine), m_duration(0), m_patternMap(engine), m_colorsetMap(engine)
{
}
SequenceStep::SequenceStep(VortexEngine &engine, uint16_t duration, const PatternMap &patternMap, const ColorsetMap &colorsetMap) :
  m_engine(engine), m_duration(duration), m_patternMap(engine, patternMap, MAP_LED_ALL), m_colorsetMap(engine, colorsetMap, MAP_LED_ALL)
{
}
SequenceStep::SequenceStep(const SequenceStep &other) :
  SequenceStep(other.m_engine, other.m_duration, other.m_patternMap, other.m_colorsetMap)
{
}

void SequenceStep::serialize(ByteStream &buffer) const
{
  buffer.serialize(m_duration);
  m_patternMap.serialize(buffer);
  m_colorsetMap.serialize(buffer);
}

void SequenceStep::unserialize(ByteStream &buffer)
{
  buffer.unserialize(&m_duration);
  m_patternMap.unserialize(buffer);
  m_colorsetMap.unserialize(buffer);
}

Sequence::Sequence(VortexEngine &engine) :
  m_engine(engine),
  m_sequenceSteps(nullptr),
  m_numSteps(0)
{
}

Sequence::~Sequence()
{
  clear();
}

Sequence::Sequence(const Sequence &other) :
  Sequence(other.m_engine)
{
  // invoke = operator
  *this = other;
}

Sequence::Sequence(Sequence &&other) noexcept :
  m_engine(other.m_engine),
  m_sequenceSteps(other.m_sequenceSteps),
  m_numSteps(other.m_numSteps)
{
  other.m_sequenceSteps = nullptr;
  other.m_numSteps = 0;
}

void Sequence::operator=(const Sequence &other)
{
  clear();
  initSteps(other.m_numSteps);
  for (uint8_t i = 0; i < other.m_numSteps; ++i) {
    m_sequenceSteps[i] = other.m_sequenceSteps[i];
  }
}

bool Sequence::operator==(const Sequence &other) const
{
  // only compare the palettes for equality
  return (m_numSteps == other.m_numSteps) &&
    (memcmp(m_sequenceSteps, other.m_sequenceSteps, m_numSteps * sizeof(SequenceStep)) == 0);
}

bool Sequence::operator!=(const Sequence &other) const
{
  return !operator==(other);
}

void Sequence::initSteps(uint8_t numSteps)
{
  if (m_sequenceSteps) {
    delete[] m_sequenceSteps;
  }
  m_sequenceSteps = new SequenceStep[numSteps] {m_engine};
  if (!m_sequenceSteps) {
    ERROR_OUT_OF_MEMORY();
    return;
  }
  m_numSteps = numSteps;
}

uint8_t Sequence::addStep(const SequenceStep &step)
{
  if (m_numSteps >= MAX_SEQUENCE_STEPS) {
    return false;
  }
  // allocate a new palette one larger than before
  SequenceStep *temp = new SequenceStep[m_numSteps + 1];
  if (!temp) {
    return false;
  }
  // if there is already some colors in the palette
  if (m_numSteps && m_sequenceSteps) {
    // copy over existing colors
    for (uint8_t i = 0; i < m_numSteps; ++i) {
      temp[i] = m_sequenceSteps[i];
    }
    // and delete the existing palette
    delete[] m_sequenceSteps;
  }
  // reassign new palette
  m_sequenceSteps = temp;
  // insert new color and increment number of colors
  m_sequenceSteps[m_numSteps] = step;
  m_numSteps++;
  return true;
}

uint8_t Sequence::addStep(uint16_t duration, const PatternMap &patternMap, const ColorsetMap &colorsetMap)
{
  return addStep(SequenceStep(duration, patternMap, colorsetMap));
}

void Sequence::clear()
{
  if (m_sequenceSteps) {
    delete[] m_sequenceSteps;
    m_sequenceSteps = nullptr;
  }
  m_numSteps = 0;
}

void Sequence::serialize(ByteStream &buffer) const
{
  buffer.serialize(m_numSteps);
  for (uint8_t i = 0; i < m_numSteps; ++i) {
    m_sequenceSteps[i].serialize(buffer);
  }
}

void Sequence::unserialize(ByteStream &buffer)
{
  buffer.unserialize(&m_numSteps);
  for (uint8_t i = 0; i < m_numSteps; ++i) {
    m_sequenceSteps[i].unserialize(buffer);
  }
}

uint8_t Sequence::numSteps() const
{
  return m_numSteps;
}

const SequenceStep &Sequence::operator[](uint8_t index) const
{
  return m_sequenceSteps[index];
}
