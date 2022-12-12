#include "RabbitPattern.h"

#include "../../Colors/Colorset.h"

RabbitPattern::RabbitPattern() :
  HybridPattern()
{
  m_patternID = PATTERN_RABBIT;
}

RabbitPattern::RabbitPattern(const PatternArgs &args) :
  RabbitPattern()
{
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
      setPatternAt(p, PATTERN_HYPERSTROBE, nullptr, &tipsColor);
    } else {
      setPatternAt(p, PATTERN_STROBE);
    }
  }
}
