#include "JestPattern.h"

JestPattern::JestPattern() :
  HybridPattern()
{
  m_patternID = PATTERN_JEST;
}

JestPattern::JestPattern(const PatternArgs &args) :
  JestPattern()
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

  // advanced pattern args for tips/tops
  PatternArgs tipArgs = { 0, 1, 5, 3, 0, 0 };
  setPatternAt(THUMB_TIP, PATTERN_ADVANCED, &tipArgs); 
  setPatternAt(INDEX_TIP, PATTERN_ADVANCED, &tipArgs);
  setPatternAt(MIDDLE_TIP, PATTERN_ADVANCED, &tipArgs);
  setPatternAt(RING_TIP, PATTERN_ADVANCED, &tipArgs);
  setPatternAt(PINKIE_TIP, PATTERN_ADVANCED, &tipArgs);

  PatternArgs topArgs = { 0, 1, 69, 3, 0, 0 };
  setPatternAt(THUMB_TOP, PATTERN_ADVANCED, &topArgs);
  setPatternAt(INDEX_TOP, PATTERN_ADVANCED, &topArgs);
  setPatternAt(MIDDLE_TOP, PATTERN_ADVANCED, &topArgs);
  setPatternAt(RING_TOP, PATTERN_ADVANCED, &topArgs);
  setPatternAt(PINKIE_TOP, PATTERN_ADVANCED, &topArgs);
}
