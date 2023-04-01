#include "SequencedPattern.h"

#include "../../Single/SingleLedPattern.h"
#include "../../PatternBuilder.h"

#include "../../../Serial/ByteStream.h"
#include "../../../Leds/Leds.h"
#include "../../../Log/Log.h"

#include <string.h>

SequencedPattern::SequencedPattern(const PatternArgs &args) :
  HybridPattern(args),
  m_sequence(),
  m_curSequence(0)
{
  // SequencedPattern is an abstract class it cannot be directly
  // instantiated so we do not need to assign a pattern id
}

SequencedPattern::SequencedPattern(const PatternArgs &args, const Sequence &sequence) :
  HybridPattern(args),
  m_sequence(sequence),
  m_curSequence(0)
{
  // SequencedPattern is an abstract class it cannot be directly
  // instantiated so we do not need to assign a pattern id
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

  // TODO: Play first sequence step in init?
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
        curPat->bind(pos);
        curPat->setColorset(curSet);
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

// apply a sequence to the pattern
void SequencedPattern::bindSequence(const Sequence &sequence)
{
  m_sequence = sequence;
}
