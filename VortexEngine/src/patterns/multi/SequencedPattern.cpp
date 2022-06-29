#include "SequencedPattern.h"

#include "../single/SingleLedPattern.h"
#include "../../PatternBuilder.h"
#include "../../SerialBuffer.h"
#include "../../Leds.h"
#include "../../Log.h"

#include <string.h>

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

void PatternMap::serialize(SerialBuffer &buffer) const
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    // ensure the PatternID is interpreted as uint8_t
    buffer.serialize((uint8_t)m_patternMap[i]);
  }
}

void PatternMap::unserialize(SerialBuffer &buffer)
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    buffer.unserialize((uint8_t *)m_patternMap + i);
  }
}

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

void ColorsetMap::serialize(SerialBuffer &buffer) const
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    m_colorsetMap[i].serialize(buffer);
  }
}

void ColorsetMap::unserialize(SerialBuffer &buffer)
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    m_colorsetMap[i].unserialize(buffer);
  }
}

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

void SequenceStep::serialize(SerialBuffer &buffer) const
{
  buffer.serialize(m_duration);
  m_patternMap.serialize(buffer);
  m_colorsetMap.serialize(buffer);
}

void SequenceStep::unserialize(SerialBuffer &buffer)
{
  buffer.unserialize(&m_duration);
  m_patternMap.unserialize(buffer);
  m_colorsetMap.unserialize(buffer);
}

SequencedPattern::SequencedPattern(uint32_t sequenceLength, const SequenceStep *sequenceSteps) :
  HybridPattern(),
  m_sequenceLength(0),
  m_sequenceSteps(nullptr),
  m_curSequence(0)
{
  m_sequenceLength = sequenceLength;
  m_sequenceSteps = new SequenceStep[m_sequenceLength];
  if (!m_sequenceSteps) {
    ERROR_OUT_OF_MEMORY();
  }
  for (uint32_t i = 0; i < m_sequenceLength; ++i) {
    m_sequenceSteps[i] = sequenceSteps[i];
  }
}

SequencedPattern::~SequencedPattern()
{
  if (m_sequenceSteps) {
    delete[] m_sequenceSteps;
  }
}

// init the pattern to initial state
void SequencedPattern::init()
{
  HybridPattern::init();

  // start the sequence at 0
  m_curSequence = 0;
  m_timer.reset();

  for (uint32_t i = 0; i < m_sequenceLength; ++i) {
    m_timer.addAlarm(m_sequenceSteps[i].m_duration);
  }

  m_timer.start();
}

// pure virtual must  the play function
void SequencedPattern::play()
{
  if (m_timer.alarm() != -1 && !m_timer.onStart()) {
    m_curSequence = (m_curSequence + 1) % m_sequenceLength;
  }
  const SequenceStep *step = m_sequenceSteps + m_curSequence;
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    const Colorset *curSet = &m_colorset;
    if (step->m_colorsetMap[pos].numColors() > 0) {
      curSet = &step->m_colorsetMap[pos];
    }
    // unset the pattern if it's wrong pattern id
    if (m_ledPatterns[pos]) {
      if (m_ledPatterns[pos]->getPatternID() != step->m_patternMap[pos] && step->m_patternMap[pos] != PATTERN_NONE) {
        delete m_ledPatterns[pos];
        m_ledPatterns[pos] = nullptr;
      } else if (!m_ledPatterns[pos]->getColorset()->equals(curSet)) {
        delete m_ledPatterns[pos];
        m_ledPatterns[pos] = nullptr;
      }
    }
    // if the pattern isnt set then initialize it
    if (!m_ledPatterns[pos] && step->m_patternMap[pos] != PATTERN_NONE) {
      m_ledPatterns[pos] = PatternBuilder::makeSingle(step->m_patternMap[pos]);
      if (m_ledPatterns[pos]) {
        m_ledPatterns[pos]->bind(curSet, pos);
        m_ledPatterns[pos]->init();
      }
    }
    if (m_ledPatterns[pos] && step->m_patternMap[pos] != PATTERN_NONE) {
      // play the pattern on this index
      m_ledPatterns[pos]->play();
    } else {
      Leds::clearIndex(pos);
    }
  }
}

// must  the serialize routine to save the pattern
void SequencedPattern::serialize(SerialBuffer &buffer) const
{
  // Note: intentionally skipping HybridPattern::serialize because we
  //       don't want to write all the sub patterns since they change.
  //       Instead we need to write all the sequencer steps
  MultiLedPattern::serialize(buffer);
  buffer.serialize(m_sequenceLength);
  for (uint32_t i = 0; i < m_sequenceLength; ++i) {
    m_sequenceSteps[i].serialize(buffer);
  }
}

void SequencedPattern::unserialize(SerialBuffer &buffer)
{
  // Note: intentionally skipping HybridPattern::unserialize
  MultiLedPattern::unserialize(buffer);
  buffer.unserialize(&m_sequenceLength);
  for (uint32_t i = 0; i < m_sequenceLength; ++i) {
    m_sequenceSteps[i].unserialize(buffer);
  }
}
