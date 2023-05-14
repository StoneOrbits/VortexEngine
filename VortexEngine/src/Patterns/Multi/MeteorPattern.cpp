#include "MeteorPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

MeteorPattern::MeteorPattern(const PatternArgs &args) :
  BlinkStepPattern(args),
  m_fadeAmount(0),
  m_randCtx(),
  m_stash()
{
  m_patternID = PATTERN_METEOR;
  REGISTER_ARG(m_fadeAmount);
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
  LedPos target = (LedPos)m_randCtx.next(LED_FIRST, LED_COUNT);
  RGBColor col = m_colorset.getNext();
  m_stash.setIndex(target, col);
}
