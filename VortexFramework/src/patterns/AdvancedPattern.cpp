#include "AdvancedPattern.h"

#include "../SerialBuffer.h"
#include "../TimeControl.h"
#include "../Colorset.h"
#include "../Leds.h"
#include "../Log.h"

AdvancedPattern::AdvancedPattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration,
                                 uint8_t groupSize, uint8_t skipCols, uint8_t repeatGroup) :
  GapPattern(onDuration, offDuration, gapDuration),
  m_groupSize(groupSize),
  m_skipCols(skipCols),
  m_repeatGroup(repeatGroup),
  m_groupCounter(0),
  m_repeatCounter(repeatGroup)
{
}

AdvancedPattern::~AdvancedPattern()
{
}

void AdvancedPattern::init()
{
  if (!m_groupSize || m_groupSize > m_colorset.numColors()) {
    m_groupSize = m_colorset.numColors();
  }
  m_groupCounter = 0;
  m_repeatCounter = m_repeatGroup;
  // run base pattern init logic
  GapPattern::init();
}

void AdvancedPattern::play()
{
  // the advanced pattern is just a gap pattern but
  // with some of the callbacks overridden to perform
  // actions at certain times in the pattern
  GapPattern::play();
}

void AdvancedPattern::triggerGap()
{
  // This is an override from GapPattern::triggerGap()
  // When the gap triggers in the gap pattern we need to 
  // reset the group counter in the advanced pattern
  // because the only way for the gap to trigger is via
  // the group counter logic in endGap
  GapPattern::triggerGap();
  m_groupCounter = 0;
}

void AdvancedPattern::endGap()
{
  // This is an override for the GapPattern callback endGap()
  GapPattern::endGap();
  // Here we perform logic for repeating groups
  if (m_repeatCounter > 0) {
    // the repeat counter starts at group size and counts down
    // each time an entire group has been displayed
    m_repeatCounter--;
    // to "repeat" we simply move the colorset back one group size
    m_colorset.skip(-(int32_t)m_groupSize);
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
  // This is overridding GapPattern::onBasicEnd which itself is
  // an override of BasicPatterns onBasicEnd callback. This is
  // so that we don't run GapPattern::onBasicEnd to prevent default 
  // gap logic at end of blinks because we will be inserting gaps
  // at different locations based on the group size
  BasicPattern::onBasicEnd();
}

void AdvancedPattern::onBlinkOff()
{
  BasicPattern::onBlinkOff();
  // count a blink in the group
  m_groupCounter++;
  // check if the group has reached the intended size
  if (m_groupCounter >= m_groupSize) {
    triggerGap();
  }
}

void AdvancedPattern::serialize(SerialBuffer &buffer) const
{
  //DEBUG("Serialize");
  GapPattern::serialize(buffer);
  //buffer.serialize(m_onDuration);
  //buffer.serialize(m_offDuration);
}

void AdvancedPattern::unserialize(SerialBuffer &buffer)
{
  //DEBUG("Unserialize");
  GapPattern::unserialize(buffer);
  //buffer.unserialize(&m_onDuration);
  //buffer.unserialize(&m_offDuration);
}
