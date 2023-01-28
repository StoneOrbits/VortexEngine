#include "ImpactPattern.h"

#include "../../Serial/ByteStream.h"
#include "../PatternBuilder.h"

ImpactPattern::ImpactPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t onDuration2,
  uint8_t offDuration2, uint8_t onDuration3, uint8_t offDuration3) :
  HybridPattern(),
  m_firstSideArgs(onDuration1, offDuration1),
  m_secondSideArgs(onDuration2, offDuration2),
  m_otherArgs(onDuration3, offDuration3)
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
  Colorset firstSideSet(m_colorset.get(0));
  Colorset secondSideSet(m_colorset.get(1));
  Colorset thirdSet(m_colorset.get(2), m_colorset.get(3), m_colorset.get(4), m_colorset.get(5), m_colorset.get(6), m_colorset.get(7));

  setPatternAt(LED_0, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_1, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_2, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_11, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_12, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_13, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_14, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_15, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_16, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_25, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_26, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);
  setPatternAt(LED_27, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstSideArgs), &firstSideSet);

  setPatternAt(LED_4, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_5, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_6, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_7, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_8, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_9, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_18, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_19, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_20, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_21, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_22, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);
  setPatternAt(LED_23, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondSideArgs), &secondSideSet);

  setPatternAt(LED_3, PatternBuilder::makeSingle(PATTERN_BASIC, &m_otherArgs), &thirdSet);
  setPatternAt(LED_10, PatternBuilder::makeSingle(PATTERN_BASIC, &m_otherArgs), &thirdSet);
  setPatternAt(LED_17, PatternBuilder::makeSingle(PATTERN_BASIC, &m_otherArgs), &thirdSet);
  setPatternAt(LED_24, PatternBuilder::makeSingle(PATTERN_BASIC, &m_otherArgs), &thirdSet);
}

void ImpactPattern::setArgs(const PatternArgs &args)
{
  HybridPattern::setArgs(args);
  m_firstSideArgs.arg1 = args.arg1;
  m_firstSideArgs.arg2 = args.arg2;
  m_secondSideArgs.arg1 = args.arg3;
  m_secondSideArgs.arg2 = args.arg4;
  m_otherArgs.arg1 = args.arg5;
  m_otherArgs.arg2 = args.arg6;
}

void ImpactPattern::getArgs(PatternArgs &args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_firstSideArgs.arg1;
  args.arg2 = m_firstSideArgs.arg2;
  args.arg3 = m_secondSideArgs.arg1;
  args.arg4 = m_secondSideArgs.arg2;
  args.arg5 = m_otherArgs.arg1;
  args.arg6 = m_otherArgs.arg2;
  args.numArgs += 6;
}
