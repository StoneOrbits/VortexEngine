#include "FlowersPattern.h"

#include "../single/BasicPattern.h"
#include "../../PatternBuilder.h"

FlowersPattern::FlowersPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t onDuration2, uint8_t offDuration2) :
  HybridPattern(),
  m_onDuration1(onDuration1),
  m_offDuration1(offDuration1),
  m_onDuration2(onDuration2),
  m_offDuration2(offDuration2)
{
}

FlowersPattern::~FlowersPattern()
{
}

// init the pattern to initial state
void FlowersPattern::init()
{
  // call base hybrid pattern init to actually initialize sub patterns
  HybridPattern::init();
  
  setPatternAt(THUMB_TIP, new BasicPattern(m_onDuration1, m_offDuration1));
  setPatternAt(THUMB_TOP, new BasicPattern(m_onDuration1, m_offDuration1));
  setPatternAt(PINKIE_TIP, new BasicPattern(m_onDuration1, m_offDuration1));
  setPatternAt(PINKIE_TOP, new BasicPattern(m_onDuration1, m_offDuration1));

  setPatternAt(MIDDLE_TIP, new BasicPattern(m_onDuration2, m_offDuration2));
  setPatternAt(MIDDLE_TOP, new BasicPattern(m_onDuration2, m_offDuration2));
  setPatternAt(INDEX_TIP, new BasicPattern(m_onDuration2, m_offDuration2));
  setPatternAt(INDEX_TOP, new BasicPattern(m_onDuration2, m_offDuration2));
  setPatternAt(RING_TOP, new BasicPattern(m_onDuration2, m_offDuration2));
  setPatternAt(RING_TIP, new BasicPattern(m_onDuration2, m_offDuration2));
}