#include "DoubleStrobePattern.h"

#include "../../Serial/ByteStream.h"

#include "../../Leds/Leds.h"

DoubleStrobePattern::DoubleStrobePattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration)
{
  m_patternID = PATTERN_DOUBLESTROBE;
}

DoubleStrobePattern::DoubleStrobePattern(const PatternArgs &args) :
  DoubleStrobePattern(args.arg1, args.arg2, args.arg3)
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
