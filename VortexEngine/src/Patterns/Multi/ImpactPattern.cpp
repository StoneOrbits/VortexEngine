#include "ImpactPattern.h"

ImpactPattern::ImpactPattern() :
  HybridPattern()
{
  m_patternID = PATTERN_IMPACT;
}

ImpactPattern::ImpactPattern(const PatternArgs &args) :
  ImpactPattern()
{
  setArgs(args);
}

ImpactPattern::~ImpactPattern()
{
}

// init the pattern to initial state
void ImpactPattern::init()
{
  // call base hybrid pattern init to actually initialize sub patterns
  HybridPattern::init();

  // thumb, middle and index/ring/pinkie
  Colorset thumb_set(m_colorset.get(0));
  Colorset middle_set(m_colorset.get(1));
  Colorset third_set(m_colorset.get(2));
  
  PatternArgs strobeArgs(25, 250);
  setPatternAt(THUMB_TIP, PATTERN_BASIC, &strobeArgs, &thumb_set);
  setPatternAt(THUMB_TOP, PATTERN_BASIC, &strobeArgs, &thumb_set);

  setPatternAt(MIDDLE_TIP, PATTERN_BASIC, &strobeArgs, &middle_set);
  setPatternAt(MIDDLE_TOP, PATTERN_BASIC, &strobeArgs, &middle_set);

  setPatternAt(INDEX_TOP, PATTERN_BASIC, &strobeArgs, &third_set);
  setPatternAt(RING_TOP, PATTERN_BASIC, &strobeArgs, &third_set);
  setPatternAt(PINKIE_TOP, PATTERN_BASIC, &strobeArgs, &third_set);

  // remaining just use default colorset
  setPatternAt(INDEX_TIP, PATTERN_STROBE);
  setPatternAt(RING_TIP, PATTERN_STROBE);
  setPatternAt(PINKIE_TIP, PATTERN_STROBE);
}
