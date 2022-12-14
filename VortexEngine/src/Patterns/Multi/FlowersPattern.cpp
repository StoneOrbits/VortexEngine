#include "FlowersPattern.h"

FlowersPattern::FlowersPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t onDuration2, uint8_t offDuration2) :
  HybridPattern(),
  m_onDuration1(onDuration1),
  m_offDuration1(offDuration1),
  m_onDuration2(onDuration2),
  m_offDuration2(offDuration2)
{
  m_patternID = PATTERN_FLOWERS;
}

FlowersPattern::FlowersPattern(const PatternArgs &args) :
  FlowersPattern()
{
  setArgs(args);
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
  Colorset sideSet(m_colorset.get(0), m_colorset.get(1));
  Colorset midSet(m_colorset.get(2), m_colorset.get(3));

  // thumb and pinkie use duration 1
  PatternArgs sideArgs(m_onDuration1, m_offDuration1);
  setPatternAt(THUMB_TIP, PATTERN_BASIC, &sideArgs, &sideSet);
  setPatternAt(THUMB_TOP, PATTERN_BASIC, &sideArgs, &sideSet);
  setPatternAt(PINKIE_TIP, PATTERN_BASIC, &sideArgs, &sideSet);
  setPatternAt(PINKIE_TOP, PATTERN_BASIC, &sideArgs, &sideSet);

  // middle index ring use duration 2
  PatternArgs midArgs(m_onDuration2, m_offDuration2);
  setPatternAt(MIDDLE_TIP, PATTERN_BASIC, &midArgs, &midSet);
  setPatternAt(MIDDLE_TOP, PATTERN_BASIC, &midArgs, &midSet);
  setPatternAt(INDEX_TIP, PATTERN_BASIC, &midArgs, &midSet);
  setPatternAt(INDEX_TOP, PATTERN_BASIC, &midArgs, &midSet);
  setPatternAt(RING_TOP, PATTERN_BASIC, &midArgs, &midSet);
  setPatternAt(RING_TIP, PATTERN_BASIC, &midArgs, &midSet);
}
