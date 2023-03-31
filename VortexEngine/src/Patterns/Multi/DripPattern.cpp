#include "DripPattern.h"

#include "../../Leds/Leds.h"

DripPattern::DripPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_sync(true)
{
  m_patternID = PATTERN_DRIP;
}

DripPattern::DripPattern(const PatternArgs &args) :
  DripPattern()
{
  setArgs(args);
}

DripPattern::~DripPattern()
{
}

void DripPattern::init()
{
  BlinkStepPattern::init();
  // reset sync bool
  m_sync = true;
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void DripPattern::blinkOn()
{
  if (!m_sync) {
    Leds::setAllEvens(m_colorset.cur());
    Leds::setAllOdds(m_colorset.peekNext());
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
