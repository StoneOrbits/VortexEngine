#include "Sequence.h"

#include "../../../Serial/ByteStream.h"
#include "../../../Memory/Memory.h"
#include "../../../Leds/Leds.h"
#include "../../../Log/Log.h"

// some arbitrary number
#define MAX_SEQUENCE_STEPS 64

PatternMap::PatternMap() :
  m_patternMap()
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    m_patternMap[i] = PATTERN_NONE;
  }
}

PatternMap::PatternMap(PatternID pattern, LedMap positions) :
  PatternMap()
{
  setPatternAt(pattern, positions);
}

// set a pattern at each position in the LedMap
void PatternMap::setPatternAt(PatternID pattern, LedMap positions)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (checkLed(positions, pos)) {
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
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    // ensure the PatternID is interpreted as uint8_t
    buffer.serialize((uint8_t)m_patternMap[i]);
  }
}

void PatternMap::unserialize(ByteStream &buffer)
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    buffer.unserialize((uint8_t *)m_patternMap + i);
  }
}

#if SAVE_TEMPLATE == 1
void PatternMap::saveTemplate(int level) const
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    IndentMsg(level, "%d,", m_patternMap[i]);
  }
}
#endif

ColorsetMap::ColorsetMap() :
  m_colorsetMap()
{
}

ColorsetMap::ColorsetMap(const Colorset &colorset, LedMap positions) :
  ColorsetMap()
{
  setColorsetAt(colorset, positions);
}

// set a pattern at each position in the LedMap
void ColorsetMap::setColorsetAt(const Colorset &colorset, LedMap positions)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (checkLed(positions, pos)) {
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
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    m_colorsetMap[i].serialize(buffer);
  }
}

void ColorsetMap::unserialize(ByteStream &buffer)
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    m_colorsetMap[i].unserialize(buffer);
  }
}

#if SAVE_TEMPLATE == 1
void ColorsetMap::saveTemplate(int level) const
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    IndentMsg(level, "{");
    m_colorsetMap[i].saveTemplate(level + 1);
    IndentMsg(level, "},");
  }
}
#endif

// Make an array of sequence steps to create a sequenced pattern
SequenceStep::SequenceStep() :
  m_duration(0), m_patternMap(), m_colorsetMap()
{
}
SequenceStep::SequenceStep(uint16_t duration, const PatternMap &patternMap, const ColorsetMap &colorsetMap) :
  m_duration(duration), m_patternMap(patternMap), m_colorsetMap(colorsetMap)
{
}
SequenceStep::SequenceStep(const SequenceStep &other) :
  SequenceStep(other.m_duration, other.m_patternMap, other.m_colorsetMap)
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

#if SAVE_TEMPLATE == 1
void SequenceStep::saveTemplate(int level) const
{
  IndentMsg(level, "\"Duration\": %d,", m_duration);
  IndentMsg(level, "\"PatternMap\": [");
  m_patternMap.saveTemplate(level + 1);
  IndentMsg(level, "],");
  IndentMsg(level, "\"ColorsetMap\": [");
  m_colorsetMap.saveTemplate(level + 1);
  IndentMsg(level, "],");
}
#endif

Sequence::Sequence() :
  m_sequenceSteps(nullptr),
  m_numSteps(0)
{
}

Sequence::~Sequence()
{
  clear();
}

Sequence::Sequence(const Sequence &other) :
  Sequence()
{
  // invoke = operator
  *this = other;
}

Sequence::Sequence(Sequence &&other) noexcept :
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
  for (uint32_t i = 0; i < other.m_numSteps; ++i) {
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

void Sequence::initSteps(uint32_t numSteps)
{
  if (m_sequenceSteps) {
    delete[] m_sequenceSteps;
  }
  m_sequenceSteps = new SequenceStep[numSteps];
  if (!m_sequenceSteps) {
    ERROR_OUT_OF_MEMORY();
    return;
  }
  m_numSteps = numSteps;
}

uint32_t Sequence::addStep(const SequenceStep &step)
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
    for (uint32_t i = 0; i < m_numSteps; ++i) {
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

uint32_t Sequence::addStep(uint32_t duration, const PatternMap &patternMap, const ColorsetMap &colorsetMap)
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
  for (uint32_t i = 0; i < m_numSteps; ++i) {
    m_sequenceSteps[i].serialize(buffer);
  }
}

void Sequence::unserialize(ByteStream &buffer)
{
  buffer.unserialize(&m_numSteps);
  for (uint32_t i = 0; i < m_numSteps; ++i) {
    m_sequenceSteps[i].unserialize(buffer);
  }
}

uint32_t Sequence::numSteps() const
{
  return m_numSteps;
}

const SequenceStep &Sequence::operator[](uint32_t index) const
{
  return m_sequenceSteps[index];
}
