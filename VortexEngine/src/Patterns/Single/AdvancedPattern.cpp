#include "AdvancedPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

AdvancedPattern::AdvancedPattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration,
                                 uint8_t groupSize, uint8_t skipCols, uint8_t repeatGroup) :
  BasicPattern(onDuration, offDuration, gapDuration),
  m_groupSize(groupSize),
  m_skipCols(skipCols),
  m_repeatGroup(repeatGroup),
  m_realGroupSize(0),
  m_groupCounter(0),
  m_repeatCounter(repeatGroup)
{
  m_patternID = PATTERN_ADVANCED;
}

AdvancedPattern::AdvancedPattern(const PatternArgs &args) :
  AdvancedPattern()
{
  setArgs(args);
}

AdvancedPattern::~AdvancedPattern()
{
}

void AdvancedPattern::init()
{
  // run base pattern init logic
  BasicPattern::init();
  if (!m_groupSize || m_groupSize > m_colorset.numColors()) {
    m_realGroupSize = m_colorset.numColors();
  } else {
    m_realGroupSize = m_groupSize;
  }
  m_groupCounter = 0;
  m_repeatCounter = m_repeatGroup;
}

void AdvancedPattern::play()
{
  // the advanced pattern is just a basic pattern but
  // with some of the callbacks overridden to perform
  // actions at certain times in the pattern
  BasicPattern::play();
}

void AdvancedPattern::triggerGap()
{
  // This is an override from BasicPattern::triggerGap()
  // When the basic triggers in the basic pattern we need to
  // reset the group counter in the advanced pattern
  // because the only way for the basic to trigger is via
  // the group counter logic in endGap
  BasicPattern::triggerGap();
  m_groupCounter = 0;
}

void AdvancedPattern::endGap()
{
  // This is an override for the BasicPattern callback endGap()
  BasicPattern::endGap();
  // Here we perform logic for repeating groups
  if (m_repeatCounter > 0) {
    // the repeat counter starts at group size and counts down
    // each time an entire group has been displayed
    m_repeatCounter--;
    // to "repeat" we simply move the colorset back one group size
    m_colorset.skip(-(int32_t)m_realGroupSize);
    // nothing more to do
    return;
  }
  if (m_skipCols > 0) {
    m_colorset.skip(m_skipCols);
  }
  if (!m_repeatCounter) {
    m_repeatCounter = m_repeatGroup;
  }
}

void AdvancedPattern::onBasicEnd()
{
  // This is overridding BasicPattern::onBasicEnd so that we don't run
  // default basic pattern logic which is responsible for triggering gaps
  // because we will be inserting the gap in other places ourselves.
}

void AdvancedPattern::onBlinkOff()
{
  BasicPattern::onBlinkOff();
  // count a blink in the group
  m_groupCounter++;
  // check if the group has reached the intended size
  if (m_groupCounter >= m_realGroupSize) {
    triggerGap();
  }
}

void AdvancedPattern::setArgs(const PatternArgs &args)
{
  BasicPattern::setArgs(args);
  m_groupSize = args.arg4;
  m_skipCols = args.arg5;
  m_repeatGroup = args.arg6;
}

void AdvancedPattern::getArgs(PatternArgs &args) const
{
  BasicPattern::getArgs(args);
  args.arg4 = m_groupSize;
  args.arg5 = m_skipCols;
  args.arg6 = m_repeatGroup;
  args.numArgs += 3;
}
