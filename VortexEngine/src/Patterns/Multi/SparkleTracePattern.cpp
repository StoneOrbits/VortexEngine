#include "SparkleTracePattern.h"

#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

SparkleTracePattern::SparkleTracePattern(const PatternArgs &args) :
  BlinkStepPattern(args),
  m_randCtx()
{
  m_patternID = PATTERN_SPARKLETRACE;
  setArgs(args);
}

SparkleTracePattern::~SparkleTracePattern()
{
}

void SparkleTracePattern::blinkOn()
{
  Leds::setAll(m_colorset.get(0));
}

void SparkleTracePattern::poststep()
{
  for (uint8_t dot = 0; dot < 4; ++dot) {
    Leds::setPair((Pair)m_randCtx.next8(PAIR_FIRST, PAIR_LAST), m_colorset.cur());
  }
  m_colorset.skip();
  if (m_colorset.curIndex() == 0) {
    m_colorset.skip();
  }
}
