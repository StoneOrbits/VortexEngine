#include "TipTopPattern.h"

#include "../PatternBuilder.h"

TipTopPattern::TipTopPattern() :
  HybridPattern()
{
  m_patternID = PATTERN_TIPTOP;
}

TipTopPattern::TipTopPattern(const PatternArgs &args) :
  TipTopPattern()
{
}

TipTopPattern::~TipTopPattern()
{
}

// init the pattern to initial state
void TipTopPattern::init()
{
  HybridPattern::init();
  Colorset dopsColor(m_colorset.get(0));
  for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
    if (isFingerTip(p)) {
      setPatternAt(p, PatternBuilder::makeSingle(PATTERN_STROBIE));
    } else {
      setPatternAt(p, PatternBuilder::makeSingle(PATTERN_DOPS), &dopsColor);
    }
  }
}
