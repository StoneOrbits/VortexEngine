#include "ImpactPattern.h"

#include "../single/BasicPattern.h"
#include "../PatternBuilder.h"

ImpactPattern::ImpactPattern() :
  HybridPattern()
{
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
  
  setPatternAt(THUMB_TIP, new BasicPattern(25, 250), &thumb_set);
  setPatternAt(THUMB_TOP, new BasicPattern(25, 250), &thumb_set);

  setPatternAt(MIDDLE_TIP, new BasicPattern(25, 250), &middle_set);
  setPatternAt(MIDDLE_TOP, new BasicPattern(25, 250), &middle_set);

  setPatternAt(INDEX_TOP, new BasicPattern(25, 250), &third_set);
  setPatternAt(RING_TOP, new BasicPattern(25, 250), &third_set);
  setPatternAt(PINKIE_TOP, new BasicPattern(25, 250), &third_set);

  // remaining just use default colorset
  setPatternAt(INDEX_TIP, PatternBuilder::makeSingle(PATTERN_STROBE));
  setPatternAt(RING_TIP, PatternBuilder::makeSingle(PATTERN_STROBE));
  setPatternAt(PINKIE_TIP, PatternBuilder::makeSingle(PATTERN_STROBE));
}
