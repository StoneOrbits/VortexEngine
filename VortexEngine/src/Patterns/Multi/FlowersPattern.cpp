#include "FlowersPattern.h"

#include "../PatternBuilder.h"
#include "../../Serial/ByteStream.h"

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

  // quadrant 1 and 3 use duration 1
  setPatternAt(LED_0, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_1, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_2, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_3, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_4, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_5, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_6, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_14, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_15, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_16, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_17, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_18, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_19, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(LED_20, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);

  // quadrant 2 and 4 use duration 2
  setPatternAt(LED_7, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_8, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_9, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_10, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_11, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_12, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_13, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_21, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_22, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_23, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_24, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_25, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_26, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(LED_27, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
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
