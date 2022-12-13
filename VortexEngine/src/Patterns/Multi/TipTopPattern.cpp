#include "TipTopPattern.h"

TipTopPattern::TipTopPattern() :
  HybridPattern()
{
  m_patternID = PATTERN_TIPTOP;
}

TipTopPattern::TipTopPattern(const PatternArgs &args) :
  TipTopPattern()
{
  setArgs(args);
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
      setPatternAt(p, PATTERN_STROBIE);
    } else {
      setPatternAt(p, PATTERN_DOPS, nullptr, &dopsColor);
    }
  }
}
