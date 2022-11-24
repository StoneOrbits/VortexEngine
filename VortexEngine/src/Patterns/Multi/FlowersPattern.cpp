#include "FlowersPattern.h"

#include "../single/BasicPattern.h"
#include "../PatternBuilder.h"

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
  
  // split colorset
  Colorset side_set(m_colorset.get(0), m_colorset.get(1));
  Colorset mid_set(m_colorset.get(2), m_colorset.get(3));

  // thumb and pinkie use duration 1
  setPatternAt(THUMB_TIP, new BasicPattern(m_onDuration1, m_offDuration1), &side_set);
  setPatternAt(THUMB_TOP, new BasicPattern(m_onDuration1, m_offDuration1), &side_set);
  setPatternAt(PINKIE_TIP, new BasicPattern(m_onDuration1, m_offDuration1), &side_set);
  setPatternAt(PINKIE_TOP, new BasicPattern(m_onDuration1, m_offDuration1), &side_set);

  // middle index ring use duration 2
  setPatternAt(MIDDLE_TIP, new BasicPattern(m_onDuration2, m_offDuration2), &mid_set);
  setPatternAt(MIDDLE_TOP, new BasicPattern(m_onDuration2, m_offDuration2), &mid_set);
  setPatternAt(INDEX_TIP, new BasicPattern(m_onDuration2, m_offDuration2), &mid_set);
  setPatternAt(INDEX_TOP, new BasicPattern(m_onDuration2, m_offDuration2), &mid_set);
  setPatternAt(RING_TOP, new BasicPattern(m_onDuration2, m_offDuration2), &mid_set);
  setPatternAt(RING_TIP, new BasicPattern(m_onDuration2, m_offDuration2), &mid_set);
}
