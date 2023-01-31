#include "CrossDopsPattern.h"

#include "../../Leds/Leds.h"

CrossDopsPattern::CrossDopsPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_switch(false)
{
  m_patternID = PATTERN_CROSSDOPS;
}

CrossDopsPattern::CrossDopsPattern(const PatternArgs &args) :
  CrossDopsPattern()
{
  setArgs(args);
}

CrossDopsPattern::~CrossDopsPattern()
{
}

// init the pattern to initial state
void CrossDopsPattern::init()
{
  BlinkStepPattern::init();
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void CrossDopsPattern::blinkOn()
{
  // set the current color on all the given leds
  if (m_switch) {
    Leds::setQuadrant(QUADRANT_1, m_colorset.cur());
    Leds::setQuadrant(QUADRANT_3, m_colorset.cur());
  } else {
    Leds::setQuadrant(QUADRANT_2, m_colorset.cur());
    Leds::setQuadrant(QUADRANT_4, m_colorset.cur());
  }
}

void CrossDopsPattern::poststep()
{
  // iterate to next color and invert of all the tops/tips
  m_colorset.getNext();
  m_switch = !m_switch;
}
