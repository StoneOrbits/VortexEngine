#include "MeteorPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include <Arduino.h>

MeteorPattern::MeteorPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration, uint8_t fadeAmount) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_fadeAmount(fadeAmount),
  m_stash()
{
  m_patternID = PATTERN_METEOR;
}

MeteorPattern::MeteorPattern(const PatternArgs &args) :
  MeteorPattern()
{
  setArgs(args);
}

MeteorPattern::~MeteorPattern()
{
}

void MeteorPattern::blinkOn()
{
  for (int i = 0; i < LED_COUNT; ++i) {
    m_stash[i].adjustBrightness(m_fadeAmount);
  }
  Leds::restoreAll(m_stash);
}

void MeteorPattern::blinkOff()
{
  Leds::clearAll();
}

void MeteorPattern::poststep()
{
  // when a new meteor is created it is incerted into the stash so the blinking pattern is not interrupted
  Pair target = (Pair)random(PAIR_FIRST, PAIR_COUNT);
  RGBColor col = m_colorset.getNext();
  m_stash.setIndex(pairEven(target), col);
  m_stash.setIndex(pairOdd(target), col);
}

void MeteorPattern::setArgs(const PatternArgs &args)
{
  BlinkStepPattern::setArgs(args);
  m_fadeAmount = args.arg4;
}

void MeteorPattern::getArgs(PatternArgs &args) const
{
  BlinkStepPattern::getArgs(args);
  args.arg4 = m_fadeAmount;
  args.numArgs += 1;
}
