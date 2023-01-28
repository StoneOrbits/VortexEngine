#include "TipTopPattern.h"

#include "../../Serial/ByteStream.h"
#include "../PatternBuilder.h"

TipTopPattern::TipTopPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t gapDuration1,
  uint8_t onDuration2, uint8_t offDuration2, uint8_t gapDuration2) :
  HybridPattern(),
  m_tipArgs(onDuration1, offDuration1, gapDuration1),
  m_topArgs(onDuration2, offDuration2, gapDuration2)
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
  Colorset tipsColor(m_colorset.get(0));
  Colorset topsColors(m_colorset.get(1), m_colorset.get(2), m_colorset.get(3), m_colorset.get(4),
    m_colorset.get(5), m_colorset.get(6), m_colorset.get(7));
  for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
    if (isPairSide(p)) {
      setPatternAt(p, PatternBuilder::makeSingle(PATTERN_BASIC, &m_tipArgs), &tipsColor);
    } else {
      setPatternAt(p, PatternBuilder::makeSingle(PATTERN_BASIC, &m_topArgs), &topsColors);
    }
  }
}

void TipTopPattern::setArgs(const PatternArgs& args)
{
  HybridPattern::setArgs(args);
  m_tipArgs.arg1 = args.arg1;
  m_tipArgs.arg2 = args.arg2;
  m_tipArgs.arg3 = args.arg3;
  m_topArgs.arg1 = args.arg4;
  m_topArgs.arg2 = args.arg5;
  m_topArgs.arg3 = args.arg6;
}

void TipTopPattern::getArgs(PatternArgs& args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_tipArgs.arg1;
  args.arg2 = m_tipArgs.arg2;
  args.arg3 = m_tipArgs.arg3;
  args.arg4 = m_topArgs.arg1;
  args.arg5 = m_topArgs.arg2;
  args.arg6 = m_topArgs.arg3;
  args.numArgs += 6;
}
