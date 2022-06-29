#include "SequencedPattern.h"

#include "../single/SingleLedPattern.h"
#include "../../PatternBuilder.h"
#include "../../Leds.h"
#include "../../Log.h"

#include <string.h>

SequencedPattern::SequencedPattern(uint32_t sequenceLength, const SequenceStep *sequenceSteps) :
  HybridPattern(),
  m_sequenceLength(sequenceLength),
  m_sequenceSteps(sequenceSteps),
  m_curSequence(0)
{
}

SequencedPattern::~SequencedPattern()
{
}

// init the pattern to initial state
void SequencedPattern::init()
{
  HybridPattern::init();

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
    if (m_ledPatterns[pos]) {
      if (m_ledPatterns[pos]->getPatternID() != step->m_patternMap[pos] && step->m_patternMap[pos] != PATTERN_NONE) {
        delete m_ledPatterns[pos];
        m_ledPatterns[pos] = nullptr;
      } else if (!m_ledPatterns[pos]->getColorset()->equals(&step->m_colorsetMap[pos])) {
        delete m_ledPatterns[pos];
        m_ledPatterns[pos] = nullptr;
      }
    }
    // if the pattern isnt set then initialize it
    if (!m_ledPatterns[pos] && step->m_patternMap[pos] != PATTERN_NONE) {
      m_ledPatterns[pos] = PatternBuilder::makeSingle(step->m_patternMap[pos]);
      if (m_ledPatterns[pos]) {
        m_ledPatterns[pos]->bind(&step->m_colorsetMap[pos], pos);
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
  HybridPattern::serialize(buffer);
}

void SequencedPattern::unserialize(SerialBuffer &buffer)
{
  HybridPattern::unserialize(buffer);
}
