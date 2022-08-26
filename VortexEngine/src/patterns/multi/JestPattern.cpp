#include "JestPattern.h"

#include "../single/AdvancedPattern.h"
#include "../../PatternBuilder.h"

JestPattern::JestPattern() :
  HybridPattern()
{
}

JestPattern::~JestPattern()
{
}

// init the pattern to initial state
void JestPattern::init()
{
  // call base hybrid pattern init to actually initialize sub patterns
  HybridPattern::init();

  setPatternAt(THUMB_TIP, new AdvancedPattern(0, 1, 5, 3, 0, 0));
  setPatternAt(INDEX_TIP, new AdvancedPattern(0, 1, 5, 3, 0, 0));
  setPatternAt(MIDDLE_TIP, new AdvancedPattern(0, 1, 5, 3, 0, 0));
  setPatternAt(RING_TIP, new AdvancedPattern(0, 1, 5, 3, 0, 0));
  setPatternAt(PINKIE_TIP, new AdvancedPattern(0, 1, 5, 3, 0, 0));

  setPatternAt(THUMB_TOP, new AdvancedPattern(0, 1, 69, 3, 0, 0));
  setPatternAt(INDEX_TOP, new AdvancedPattern(0, 1, 69, 3, 0, 0));
  setPatternAt(MIDDLE_TOP, new AdvancedPattern(0, 1, 69, 3, 0, 0));
  setPatternAt(RING_TOP, new AdvancedPattern(0, 1, 69, 3, 0, 0));
  setPatternAt(PINKIE_TOP, new AdvancedPattern(0, 1, 69, 3, 0, 0));
}