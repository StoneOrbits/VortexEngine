#include "DoubleStrobePattern.h"

#include "../../SerialBuffer.h"
#include "../../Leds.h"
#include "../../Timings.h"

DoubleStrobePattern::DoubleStrobePattern() :
  BlinkStepPattern(DOPS_ON_DURATION, DOPS_OFF_DURATION, 100)
{
}

DoubleStrobePattern::~DoubleStrobePattern()
{
}

void DoubleStrobePattern::init()
{
  BlinkStepPattern::init();
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void DoubleStrobePattern::blinkOn()
{
  Leds::setAllTops(m_colorset.cur());
  Leds::setAllTips(m_colorset.peekNext());
}
