#include "RabbitPattern.h"

#include "../../Colors/Colorset.h"
#include "../PatternBuilder.h"

RabbitPattern::RabbitPattern() :
  HybridPattern()
{
  m_patternID = PATTERN_RABBIT;
}

RabbitPattern::RabbitPattern(const PatternArgs &args) :
  RabbitPattern()
{
  setArgs(args);
}

RabbitPattern::~RabbitPattern()
{
}

// init the pattern to initial state
void RabbitPattern::init()
{
  HybridPattern::init();
  Colorset tipsColor(m_colorset.get(0));
  for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
    if (isFingerTip(p)) {
      //tipsArgs = { onDuration, offDuration, gapDuration};
      //setPatternAt(p, PatternBuilder::makeSingle(PATTERN_BASIC, &args), &tipsColor);
    } else {
      //topsArgs = { onDuration2, offDuration2, gapDuration2 };
      //setPatternAt(p, PatternBuilder::makeSingle(PATTERN_BASIC, &args2));
    }
  }
}
