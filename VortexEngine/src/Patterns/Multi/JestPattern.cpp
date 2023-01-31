#include "JestPattern.h"

JestPattern::JestPattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration1,
  uint8_t gapDuration2, uint8_t groupSize) :
  HybridPattern(),
  m_tipArgs(onDuration, offDuration, gapDuration1, groupSize, 0, 0),
  m_topArgs(onDuration, offDuration, gapDuration2, groupSize, 0, 0)
{
  m_patternID = PATTERN_JEST;
}

JestPattern::JestPattern(const PatternArgs &args) :
  JestPattern()
{
  setArgs(args);
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
  setPatternAt(THUMB_TIP, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(INDEX_TIP, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(MIDDLE_TIP, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(RING_TIP, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(PINKIE_TIP, PATTERN_ADVANCED, &m_tipArgs);

  setPatternAt(THUMB_TOP, PATTERN_ADVANCED, &m_topArgs);
  setPatternAt(INDEX_TOP, PATTERN_ADVANCED, &m_topArgs);
  setPatternAt(MIDDLE_TOP, PATTERN_ADVANCED, &m_topArgs);
  setPatternAt(RING_TOP, PATTERN_ADVANCED, &m_topArgs);
  setPatternAt(PINKIE_TOP, PATTERN_ADVANCED, &m_topArgs);
}

void JestPattern::setArgs(const PatternArgs &args)
{
  HybridPattern::setArgs(args);
  m_tipArgs.arg1 = args.arg1;
  m_tipArgs.arg2 = args.arg2;
  m_tipArgs.arg3 = args.arg3;
  m_topArgs.arg3 = args.arg4;
  m_topArgs.arg4 = args.arg5;
}

void JestPattern::getArgs(PatternArgs &args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_tipArgs.arg1;
  args.arg2 = m_tipArgs.arg2;
  args.arg3 = m_tipArgs.arg3;
  args.arg4 = m_topArgs.arg3;
  args.arg5 = m_topArgs.arg4;
  args.numArgs += 5;
}
