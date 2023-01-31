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
  Colorset set1(m_colorset.get(0), m_colorset.get(1));
  Colorset set2(m_colorset.get(2), m_colorset.get(3));

  // quadrant 1 and 3 use duration 1
  for (LedPos led = LED_FIRST; led < LED_COUNT; ++led) {
    if (led < LED_7 || (led > LED_13 && led < LED_21)) {
      // 1-6 and 14-20
      setPatternAt(led, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &set1);
    } else {
      // 7-13 and 21-27
      setPatternAt(led, PatternBuilder::makeSingle(PATTERN_BASIC, &m_secondPatternArgs), &set2);
    }
  }
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
