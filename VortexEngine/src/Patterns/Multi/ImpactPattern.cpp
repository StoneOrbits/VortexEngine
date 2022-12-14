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
  Colorset thumbSet(m_colorset.get(0));
  Colorset middleSet(m_colorset.get(1));
  Colorset thirdSet(m_colorset.get(2));
  
  PatternArgs strobeArgs(25, 250);
  setPatternAt(THUMB_TIP, PATTERN_BASIC, &strobeArgs, &thumbSet);
  setPatternAt(THUMB_TOP, PATTERN_BASIC, &strobeArgs, &thumbSet);

  setPatternAt(MIDDLE_TIP, PATTERN_BASIC, &strobeArgs, &middleSet);
  setPatternAt(MIDDLE_TOP, PATTERN_BASIC, &strobeArgs, &middleSet);

  setPatternAt(INDEX_TOP, PATTERN_BASIC, &strobeArgs, &thirdSet);
  setPatternAt(RING_TOP, PATTERN_BASIC, &strobeArgs, &thirdSet);
  setPatternAt(PINKIE_TOP, PATTERN_BASIC, &strobeArgs, &thirdSet);

  // remaining just use default colorset
  setPatternAt(INDEX_TIP, PATTERN_STROBE);
  setPatternAt(RING_TIP, PATTERN_STROBE);
  setPatternAt(PINKIE_TIP, PATTERN_STROBE);
}
