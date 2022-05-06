#include "AdvancedPattern.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Log.h"

AdvancedPattern::AdvancedPattern(uint32_t onDuration, uint32_t offDuration, uint32_t gapDuration,
                                 uint32_t groupSize, uint32_t skipCols, uint32_t repeatGroup) :
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

void AdvancedPattern::init(Colorset *colorset, LedPos pos)
{
  if (!m_groupSize || m_groupSize > colorset->numColors()) {
    m_groupSize = colorset->numColors();
  }
  m_groupCounter = 0;
  m_repeatCounter = m_repeatGroup;
  // run base pattern init logic
  GapPattern::init(colorset, pos);
}

void AdvancedPattern::play()
{
  GapPattern::play();
}

void AdvancedPattern::triggerGap()
{
  GapPattern::triggerGap();
  m_groupCounter = 0;
}

void AdvancedPattern::endGap()
{
  GapPattern::endGap();
  if (m_repeatCounter > 0) {
    m_repeatCounter--;
    // move backwards one group size
    m_pColorset->skip(-(int32_t)m_groupSize);
    return;
  }
  if (m_skipCols > 0) {
    m_pColorset->skip(m_skipCols);
  }
  if (!m_repeatCounter) {
    m_repeatCounter = m_repeatGroup;
  }
}

void AdvancedPattern::onBasicStart()
{
}

void AdvancedPattern::onBasicEnd()
{
  BasicPattern::onBasicEnd();
  // don't run GapPattern::onBasicEnd to prevent default gap logic at end of blinks
}

void AdvancedPattern::onBlinkOn()
{
  BasicPattern::onBlinkOn();
  // count a blink in the group
  m_groupCounter++;
}

void AdvancedPattern::onBlinkOff()
{
  BasicPattern::onBlinkOff();
  // count a blink in the group
  if (m_groupCounter >= m_groupSize) {
    triggerGap();
  }
}

void AdvancedPattern::serialize() const
{
  GapPattern::serialize();
  Serial.print(m_onDuration);
  Serial.print(m_offDuration);
}

void AdvancedPattern::unserialize()
{
}
