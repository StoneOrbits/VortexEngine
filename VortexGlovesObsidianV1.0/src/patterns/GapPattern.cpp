#include "GapPattern.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Log.h"

GapPattern::GapPattern(uint32_t onDuration, uint32_t offDuration, uint32_t gapDuration) :
  BasicPattern(onDuration, offDuration),
  m_gapDuration(gapDuration),
  m_gapTimer(),
  m_inGap(false)
{
}

GapPattern::~GapPattern()
{
}

void GapPattern::init(Colorset *set, LedPos pos)
{
  m_gapTimer.reset();
  m_gapTimer.addAlarm(m_gapDuration);
  // don't need to start the gap timer right now
  BasicPattern::init(set, pos);
}

void GapPattern::play()
{
  if (m_inGap) {
    // check to see if the gap ended yet
    if (m_gapTimer.alarm() != 0) {
      // if not then continue clearing the led
      Leds::clearIndex(m_ledPos);
      return;
    }
    endGap();
  } else if (onEnd()) {
    triggerGap();
  } 
  BasicPattern::play();
}

void GapPattern::serialize() const
{
}

void GapPattern::unserialize()
{
}

void GapPattern::triggerGap()
{
  // after the pattern ends start the gap
  m_gapTimer.restart();
  m_inGap = true;
}

void GapPattern::endGap()
{
  // otherwise the timer triggered, end the gap
  m_inGap = false;
  // resume the pattern so that it's timers start from now
  BasicPattern::resume();
}
