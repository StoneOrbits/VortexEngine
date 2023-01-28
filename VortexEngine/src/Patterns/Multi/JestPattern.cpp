#include "JestPattern.h"
#include "../../Serial/ByteStream.h"

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
  setPatternAt(LED_0, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_1, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_2, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_4, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_5, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_6, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_7, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_8, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_9, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_11, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_12, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_13, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_14, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_15, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_16, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_18, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_19, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_20, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_21, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_22, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_23, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_25, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_26, PATTERN_ADVANCED, &m_tipArgs);
  setPatternAt(LED_27, PATTERN_ADVANCED, &m_tipArgs);

  setPatternAt(LED_3, PATTERN_ADVANCED, &m_topArgs);
  setPatternAt(LED_10, PATTERN_ADVANCED, &m_topArgs);
  setPatternAt(LED_17, PATTERN_ADVANCED, &m_topArgs);
  setPatternAt(LED_24, PATTERN_ADVANCED, &m_topArgs);
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
