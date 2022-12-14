#include "SparkleTracePattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

SparkleTracePattern::SparkleTracePattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration)
{
  m_patternID = PATTERN_SPARKLETRACE;
}

SparkleTracePattern::SparkleTracePattern(const PatternArgs &args) :
  SparkleTracePattern()
{
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
  for (int dot = 0; dot < 4; ++dot) {
    Leds::setFinger((Finger)random(FINGER_FIRST, FINGER_LAST + 1), m_colorset.cur());
  }
  m_colorset.skip();
  if (m_colorset.curIndex() == 0) {
    m_colorset.skip();
  }
}
