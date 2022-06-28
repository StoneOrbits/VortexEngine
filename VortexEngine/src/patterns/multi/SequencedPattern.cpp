#include "SequencedPattern.h"

#include "../single/SingleLedPattern.h"
#include "../../PatternBuilder.h"
#include "../../Leds.h"
#include "../../Log.h"

#include <string.h>

SequencedPattern::SequencedPattern(uint32_t sequenceLength, const SequenceStep *sequenceSteps) :
  MultiLedPattern(),
  m_sequenceLength(sequenceLength),
  m_sequenceSteps(sequenceSteps),
  m_curSequence(0),
  m_curPatterns{0}
{
}

SequencedPattern::~SequencedPattern()
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    if (m_curPatterns[i]) {
      delete m_curPatterns[i];
    }
  }
}

// init the pattern to initial state
void SequencedPattern::init()
{
  MultiLedPattern::init();

  // let the sequence wrap around to 0 on first alarm
  m_curSequence = (uint32_t)-1;
  m_timer.reset();
  
  for (uint32_t i = 0; i < m_sequenceLength; ++i) {
    m_timer.addAlarm(m_sequenceSteps[i].m_duration);
  }

  m_timer.start();
}

// pure virtual must  the play function
void SequencedPattern::play()
{
  if (m_timer.alarm() != -1) {
    m_curSequence = (m_curSequence + 1) % m_sequenceLength;
  }
  const SequenceStep *step = m_sequenceSteps + m_curSequence;
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // unset the pattern if it's wrong pattern id
    if (m_curPatterns[pos]) {
      if (m_curPatterns[pos]->getPatternID() != step->m_patternMap[pos] && step->m_patternMap[pos] != PATTERN_NONE) {
        delete m_curPatterns[pos];
        m_curPatterns[pos] = nullptr;
      } else if (!m_curPatterns[pos]->getColorset()->equals(&step->m_colorsetMap[pos])) {
        delete m_curPatterns[pos];
        m_curPatterns[pos] = nullptr;
      }
    }
    // if the pattern isnt set then initialize it
    if (!m_curPatterns[pos] && step->m_patternMap[pos] != PATTERN_NONE) {
      m_curPatterns[pos] = PatternBuilder::makeSingle(step->m_patternMap[pos]);
      if (m_curPatterns[pos]) {
        m_curPatterns[pos]->bind(&step->m_colorsetMap[pos], pos);
        m_curPatterns[pos]->init();
      }
    }
    if (m_curPatterns[pos] && step->m_patternMap[pos] != PATTERN_NONE) {
      // play the pattern on this index
      m_curPatterns[pos]->play();
    } else {
      Leds::clearIndex(pos);
    }
  }
}

// must  the serialize routine to save the pattern
void SequencedPattern::serialize(SerialBuffer &buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void SequencedPattern::unserialize(SerialBuffer &buffer)
{
  MultiLedPattern::unserialize(buffer);
}
