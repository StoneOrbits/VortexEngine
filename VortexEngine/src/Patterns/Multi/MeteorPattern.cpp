#include "MeteorPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

MeteorPattern::MeteorPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration, uint8_t fadeAmount) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_fadeAmount(fadeAmount),
  m_stash()
{
  m_patternID = PATTERN_METEOR;
}

MeteorPattern::MeteorPattern(const PatternArgs &args) :
  MeteorPattern(args.arg1, args.arg2, args.arg3)
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
void MeteorPattern::serialize(ByteStream& buffer) const
{
  BlinkStepPattern::serialize(buffer);
  buffer.serialize(m_fadeAmount);
}

void MeteorPattern::unserialize(ByteStream& buffer)
{
  BlinkStepPattern::unserialize(buffer);
  buffer.unserialize(&m_fadeAmount);
}

#if SAVE_TEMPLATE == 1
void MeteorPattern::saveTemplate(int level) const
{
  BlinkStepPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"FadeAmount\": %d,", m_fadeAmount);
}
#endif


