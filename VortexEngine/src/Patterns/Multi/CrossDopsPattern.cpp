#include "CrossDopsPattern.h"

#include "../../Leds/Leds.h"

CrossDopsPattern::CrossDopsPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_ledMap(0)
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
  // Alternating evens and odds mapping of leds to turn on/off
  m_ledMap = MAP_LED(LED_9) | MAP_LED(LED_6) | MAP_LED(LED_5) | MAP_LED(LED_2) | MAP_LED(LED_1);
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void CrossDopsPattern::blinkOn()
{
  // set the current color on all the given leds
  Leds::setMap(m_ledMap, m_colorset.cur());
}

void CrossDopsPattern::poststep()
{
  // iterate to next color and invert of all the evens/odds
  m_colorset.getNext();
  m_ledMap = MAP_INVERSE(m_ledMap);
}
