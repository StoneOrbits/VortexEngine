#include "FlowersPattern.h"

FlowersPattern::FlowersPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t gapDuration1,
  uint8_t onDuration2, uint8_t offDuration2, uint8_t gapDuration2) :
  HybridPattern(),
  m_firstPatternArgs(onDuration1, offDuration1, gapDuration1),
  m_secondPatternArgs(onDuration2, offDuration2, gapDuration2)
{
  m_patternID = PATTERN_FLOWERS;
}

FlowersPattern::FlowersPattern(const PatternArgs &args) :
  FlowersPattern()
{
  setArgs(args);
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
  Colorset sideSet(m_colorset.get(0), m_colorset.get(1));
  Colorset midSet(m_colorset.get(2), m_colorset.get(3));

  // thumb and pinkie use duration 1
  setPatternAt(LED_0, PATTERN_BASIC, &m_firstPatternArgs, &sideSet);
  setPatternAt(LED_1, PATTERN_BASIC, &m_firstPatternArgs, &sideSet);
  setPatternAt(LED_8, PATTERN_BASIC, &m_firstPatternArgs, &sideSet);
  setPatternAt(LED_9, PATTERN_BASIC, &m_firstPatternArgs, &sideSet);

  // middle index ring use duration 2
  setPatternAt(LED_2, PATTERN_BASIC, &m_firstPatternArgs, &midSet);
  setPatternAt(LED_3, PATTERN_BASIC, &m_firstPatternArgs, &midSet);
  setPatternAt(LED_4, PATTERN_BASIC, &m_firstPatternArgs, &midSet);
  setPatternAt(LED_5, PATTERN_BASIC, &m_firstPatternArgs, &midSet);
  setPatternAt(LED_6, PATTERN_BASIC, &m_firstPatternArgs, &midSet);
  setPatternAt(LED_7, PATTERN_BASIC, &m_firstPatternArgs, &midSet);
}

void FlowersPattern::setArgs(const PatternArgs &args)
{
  HybridPattern::setArgs(args);
  m_firstPatternArgs.arg1 = args.arg1;
  m_firstPatternArgs.arg2 = args.arg2;
  m_firstPatternArgs.arg3 = args.arg3;
  m_secondPatternArgs.arg1 = args.arg4;
  m_secondPatternArgs.arg2 = args.arg5;
  m_secondPatternArgs.arg3 = args.arg6;
}

void FlowersPattern::getArgs(PatternArgs &args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_firstPatternArgs.arg1;
  args.arg2 = m_firstPatternArgs.arg2;
  args.arg3 = m_firstPatternArgs.arg3;
  args.arg4 = m_secondPatternArgs.arg1;
  args.arg5 = m_secondPatternArgs.arg2;
  args.arg6 = m_secondPatternArgs.arg3;
  args.numArgs += 6;
}
