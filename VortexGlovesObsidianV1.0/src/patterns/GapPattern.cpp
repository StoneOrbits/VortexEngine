#include "GapPattern.h"

#include "../SerialBuffer.h"
#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Log.h"

GapPattern::GapPattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration) :
  BasicPattern(onDuration, offDuration),
  m_gapDuration(gapDuration),
  m_gapTimer(),
  m_inGap(false)
{
}

GapPattern::~GapPattern()
{
}

void GapPattern::init()
{
  m_gapTimer.reset();
  m_gapTimer.addAlarm(m_gapDuration);

  m_inGap = false;

  // don't need to start the gap timer right now
  BasicPattern::init();
}

void GapPattern::play()
{
  if (m_inGap) {
    // check to see if the gap timer triggered to end the gap
    if (m_gapTimer.alarm() == 0) {
      endGap();
    }
    Leds::clearIndex(m_ledPos);
  } else {
    BasicPattern::play();
  }
}

void GapPattern::onBasicEnd()
{
  triggerGap();
}

void GapPattern::serialize(SerialBuffer &buffer) const
{
  //DEBUG("Serialize");
  BasicPattern::serialize(buffer);
  //buffer.serialize(m_gapDuration);
}

void GapPattern::unserialize(SerialBuffer &buffer)
{
  //DEBUG("Unserialize");
  BasicPattern::unserialize(buffer);
  //buffer.unserialize(&m_gapDuration);
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
}
