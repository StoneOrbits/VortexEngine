#include "ImpactPattern.h"

#include "../../Serial/ByteStream.h"
#include "../PatternBuilder.h"

ImpactPattern::ImpactPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t onDuration2,
  uint8_t offDuration2, uint8_t onDuration3, uint8_t offDuration3) :
  HybridPattern(),
  m_thumbArgs(onDuration1, offDuration1),
  m_middleArgs(onDuration2, offDuration2),
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
  Colorset thumbSet(m_colorset.get(0));
  Colorset middleSet(m_colorset.get(1));
  Colorset thirdSet(m_colorset.get(2));
  Colorset fourthSet(m_colorset.get(3), m_colorset.get(4), m_colorset.get(5), m_colorset.get(6), m_colorset.get(7));

  setPatternAt(THUMB_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_thumbArgs), &thumbSet);
  setPatternAt(THUMB_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_thumbArgs), &thumbSet);

  setPatternAt(MIDDLE_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_middleArgs), &middleSet);
  setPatternAt(MIDDLE_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_middleArgs), &middleSet);

  setPatternAt(INDEX_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_otherArgs), &thirdSet);
  setPatternAt(RING_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_otherArgs), &thirdSet);
  setPatternAt(PINKIE_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_otherArgs), &thirdSet);

  setPatternAt(INDEX_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_middleArgs), &fourthSet);
  setPatternAt(RING_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_middleArgs), &fourthSet);
  setPatternAt(PINKIE_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_middleArgs), &fourthSet);
}

void ImpactPattern::setArgs(const PatternArgs& args)
{
  HybridPattern::setArgs(args);
  m_thumbArgs.arg1 = args.arg1;
  m_thumbArgs.arg2 = args.arg2;
  m_middleArgs.arg1 = args.arg3;
  m_middleArgs.arg2 = args.arg4;
  m_otherArgs.arg1 = args.arg5;
  m_otherArgs.arg2 = args.arg6;
}

void ImpactPattern::getArgs(PatternArgs& args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_thumbArgs.arg1;
  args.arg2 = m_thumbArgs.arg2;
  args.arg3 = m_middleArgs.arg1;
  args.arg4 = m_middleArgs.arg2;
  args.arg5 = m_otherArgs.arg1;
  args.arg6 = m_otherArgs.arg2;
  args.numArgs += 6;
}
