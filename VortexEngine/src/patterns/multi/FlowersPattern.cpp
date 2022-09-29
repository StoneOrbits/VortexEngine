#include "FlowersPattern.h"

#include "../single/BasicPattern.h"
#include "../../PatternBuilder.h"

FlowersPattern::FlowersPattern() :
  HybridPattern()
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
  
  setPatternAt(THUMB_TIP, new BasicPattern(5, 25));
  setPatternAt(THUMB_TOP, new BasicPattern(5, 25));  
  setPatternAt(PINKIE_TIP, new BasicPattern(5, 25));
  setPatternAt(PINKIE_TOP, new BasicPattern(5, 25));

  setPatternAt(MIDDLE_TIP, new BasicPattern(3, 5));
  setPatternAt(MIDDLE_TOP, new BasicPattern(3, 5));
  setPatternAt(INDEX_TIP, new BasicPattern(3, 5));
  setPatternAt(INDEX_TOP, new BasicPattern(3, 5));
  setPatternAt(RING_TOP, new BasicPattern(3, 5));
  setPatternAt(RING_TIP, new BasicPattern(3, 5));
}