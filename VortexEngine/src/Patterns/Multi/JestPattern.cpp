#include "JestPattern.h"

JestPattern::JestPattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration1,
  uint8_t gapDuration2, uint8_t groupSize) :
  HybridPattern(),
  m_evenArgs(onDuration, offDuration, gapDuration1, groupSize, 0, 0),
  m_oddArgs(onDuration, offDuration, gapDuration2, groupSize, 0, 0)
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

  // advanced pattern args for evens/odds
  setPatternAt(LED_8, PATTERN_ADVANCED, &m_evenArgs);
  setPatternAt(LED_6, PATTERN_ADVANCED, &m_evenArgs);
  setPatternAt(LED_4, PATTERN_ADVANCED, &m_evenArgs);
  setPatternAt(LED_2, PATTERN_ADVANCED, &m_evenArgs);
  setPatternAt(LED_0, PATTERN_ADVANCED, &m_evenArgs);

  setPatternAt(LED_9, PATTERN_ADVANCED, &m_oddArgs);
  setPatternAt(LED_7, PATTERN_ADVANCED, &m_oddArgs);
  setPatternAt(LED_5, PATTERN_ADVANCED, &m_oddArgs);
  setPatternAt(LED_3, PATTERN_ADVANCED, &m_oddArgs);
  setPatternAt(LED_1, PATTERN_ADVANCED, &m_oddArgs);
}

void JestPattern::setArgs(const PatternArgs &args)
{
  HybridPattern::setArgs(args);
  m_evenArgs.arg1 = args.arg1;
  m_evenArgs.arg2 = args.arg2;
  m_evenArgs.arg3 = args.arg3;
  m_oddArgs.arg3 = args.arg4;
  m_oddArgs.arg4 = args.arg5;
}

void JestPattern::getArgs(PatternArgs &args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_evenArgs.arg1;
  args.arg2 = m_evenArgs.arg2;
  args.arg3 = m_evenArgs.arg3;
  args.arg4 = m_oddArgs.arg3;
  args.arg5 = m_oddArgs.arg4;
  args.numArgs += 5;
}
