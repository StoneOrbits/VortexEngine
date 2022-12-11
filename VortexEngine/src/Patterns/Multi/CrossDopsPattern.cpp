#include "CrossDopsPattern.h"

#include "../../Leds/Leds.h"

CrossDopsPattern::CrossDopsPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_ledMap(0)
{
  m_patternID = PATTERN_CROSSDOPS;
}

CrossDopsPattern::CrossDopsPattern(const PatternArgs &args) :
  CrossDopsPattern(args.arg1, args.arg2, args.arg3)
{
}

CrossDopsPattern::~CrossDopsPattern()
{
}

// init the pattern to initial state
void CrossDopsPattern::init()
{
  BlinkStepPattern::init();
  // Alternating tops and tips mapping of leds to turn on/off
  m_ledMap = MAP_LED(THUMB_TOP) | MAP_LED(INDEX_TIP) | MAP_LED(MIDDLE_TOP) | MAP_LED(RING_TIP) | MAP_LED(PINKIE_TOP);
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
  // iterate to next color and invert of all the tops/tips
  m_colorset.getNext();
  m_ledMap = MAP_INVERSE(m_ledMap);
}
