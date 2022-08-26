#include "MeteorPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

MeteorPattern::MeteorPattern() :
  BlinkStepPattern(5, 8, 50),
  m_stash()
{
}

MeteorPattern::~MeteorPattern()
{
}

void MeteorPattern::blinkOn()
{
  Leds::restoreAll(m_stash);
  Leds::adjustBrightnessAll(15);
}

void MeteorPattern::blinkOff()
{
  Leds::stashAll(m_stash);
  Leds::clearAll();
}

void MeteorPattern::poststep()
{
  Leds::setFinger((Finger)random(FINGER_FIRST, FINGER_COUNT), m_colorset.getNext());
}
