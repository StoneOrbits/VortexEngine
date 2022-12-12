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
  FlowersPattern(args.arg1, args.arg2, args.arg3)
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
  PatternArgs sideArgs(m_onDuration1, m_offDuration1);
  setPatternAt(THUMB_TIP, PATTERN_BASIC, &sideArgs, &side_set);
  setPatternAt(THUMB_TOP, PATTERN_BASIC, &sideArgs, &side_set);
  setPatternAt(PINKIE_TIP, PATTERN_BASIC, &sideArgs, &side_set);
  setPatternAt(PINKIE_TOP, PATTERN_BASIC, &sideArgs, &side_set);

  // middle index ring use duration 2
  PatternArgs midArgs(m_onDuration2, m_offDuration2);
  setPatternAt(MIDDLE_TIP, PATTERN_BASIC, &midArgs, &mid_set);
  setPatternAt(MIDDLE_TOP, PATTERN_BASIC, &midArgs, &mid_set);
  setPatternAt(INDEX_TIP, PATTERN_BASIC, &midArgs, &mid_set);
  setPatternAt(INDEX_TOP, PATTERN_BASIC, &midArgs, &mid_set);
  setPatternAt(RING_TOP, PATTERN_BASIC, &midArgs, &mid_set);
  setPatternAt(RING_TIP, PATTERN_BASIC, &midArgs, &mid_set);
}
