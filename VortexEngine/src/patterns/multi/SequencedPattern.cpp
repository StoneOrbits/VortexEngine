#include "SequencedPattern.h"

#include "../single/SingleLedPattern.h"
#include "../../PatternBuilder.h"
#include "../../SerialBuffer.h"
#include "../../Leds.h"
#include "../../Log.h"

#include <string.h>

SequencedPattern::SequencedPattern(const Sequence &sequence) :
  HybridPattern(),
  m_sequence(sequence),
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

  // start the sequence at 0
  m_curSequence = 0;
  m_timer.reset();

  // create an alarm for each duration in the sequence
  for (uint32_t i = 0; i < m_sequence.numSteps(); ++i) {
    m_timer.addAlarm(m_sequence[i].m_duration);
  }

  m_timer.start();
}

// pure virtual must  the play function
void SequencedPattern::play()
{
  if (m_timer.alarm() != -1 && !m_timer.onStart()) {
    m_curSequence = (m_curSequence + 1) % m_sequence.numSteps();
  }
  const SequenceStep &step = m_sequence[m_curSequence];
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // the current initialized pattern for this LED
    SingleLedPattern *curPat = m_ledPatterns[pos];
    // the intended pattern ID for this LED on this step
    PatternID stepPattern = step.m_patternMap[pos];
    // the actual colorset being used might be the pattern's colorset
    const Colorset *curSet = &m_colorset;
    if (step.m_colorsetMap[pos].numColors() > 0) {
      // or it might be the steps' colorset if it's not empty
      curSet = &step.m_colorsetMap[pos];
    }
    // if there's no pattern, or pattern is wrong colorset or pattern ID
    if (!curPat || !curPat->getColorset()->equals(curSet) || curPat->getPatternID() != stepPattern) {
      // delete any existing pattern and re-create it
      delete m_ledPatterns[pos];
      // create whichever pattern this step is (maybe PATTERN_NONE)
      curPat = m_ledPatterns[pos] = PatternBuilder::makeSingle(stepPattern);
      // if a pattern was created then bind and init it
      if (curPat) {
        curPat->bind(curSet, pos);
        curPat->init();
      }
    }
    // if there's no pattern set, or the intended pattern of this step is NONE
    if (!curPat || stepPattern == PATTERN_NONE) {
      // clear the LED and don't play the pattern
      Leds::clearIndex(pos);
      continue;
    }
    // otherwise play the pattern on this index
    curPat->play();
  }
}

// must  the serialize routine to save the pattern
void SequencedPattern::serialize(SerialBuffer &buffer) const
{
  // Note: intentionally skipping HybridPattern::serialize because we
  //       don't want to write all the sub patterns since they change.
  //       Instead we need to write all the sequencer steps
  MultiLedPattern::serialize(buffer);
  m_sequence.serialize(buffer);
}

void SequencedPattern::unserialize(SerialBuffer &buffer)
{
  // Note: intentionally skipping HybridPattern::unserialize
  MultiLedPattern::unserialize(buffer);
  m_sequence.unserialize(buffer);
}
