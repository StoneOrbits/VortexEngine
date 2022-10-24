#include "MeteorPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

MeteorPattern::MeteorPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration, uint8_t fadeAmount) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_fadeAmount(fadeAmount),
  m_stash()
{
}

MeteorPattern::~MeteorPattern()
{
}

void MeteorPattern::blinkOn()
{
  Leds::restoreAll(m_stash);
  Leds::adjustBrightnessAll(m_fadeAmount);
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

// must override the serialize routine to save the pattern
void MeteorPattern::serialize(SerialBuffer& buffer) const
{
  BlinkStepPattern::serialize(buffer);
  buffer.serialize(m_fadeAmount);
}

void MeteorPattern::unserialize(SerialBuffer& buffer)
{
  BlinkStepPattern::unserialize(buffer);
  buffer.unserialize(&m_fadeAmount);
}
