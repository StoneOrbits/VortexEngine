#include "DripPattern.h"

#include "../../Leds.h"

DripPattern::DripPattern() :
  BlinkStepPattern(10, 10, 100),
  m_sync(true)
{
}

DripPattern::~DripPattern()
{
}

void DripPattern::init()
{
  BlinkStepPattern::init();
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void DripPattern::blinkOn()
{
  if (!m_sync) {
    Leds::setAllTips(m_colorset.cur());
    Leds::setAllTops(m_colorset.peekNext());
  } else {
    Leds::setAll(m_colorset.cur());
  }
}

void DripPattern::poststep()
{
  m_sync = !m_sync;
  if (m_sync) {
    m_colorset.getNext();
  }
}
