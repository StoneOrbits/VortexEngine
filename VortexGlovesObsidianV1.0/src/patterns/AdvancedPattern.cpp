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
  m_repeatGroup(repeatGroup)
{
}

AdvancedPattern::~AdvancedPattern()
{
}

void AdvancedPattern::init(Colorset *colorset, LedPos pos)
{
  // run base pattern init logic
  GapPattern::init(colorset, pos);
}

void AdvancedPattern::play()
{
  if (!inGap() && m_groupSize && ((m_pColorset->curIndex() + 1) % m_groupSize) == 0 && m_blinkTimer.onEnd()) {
    triggerGap();
    return;
  }
  if (inGap()) {
    GapPattern::play();
  } else {
    BasicPattern::play();
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
