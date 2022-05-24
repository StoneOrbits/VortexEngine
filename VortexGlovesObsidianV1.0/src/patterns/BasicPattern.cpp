#include "BasicPattern.h"

#include "../SerialBuffer.h"
#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Log.h"

BasicPattern::BasicPattern(uint32_t onDuration, uint32_t offDuration) :
  SingleLedPattern(),
  m_onDuration(onDuration),
  m_offDuration(offDuration),
  m_blinkTimer()
{
}

BasicPattern::~BasicPattern()
{
}

void BasicPattern::init()
{
  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_onDuration);
  m_blinkTimer.addAlarm(m_offDuration);

  // start the blink timer from the current frame
  m_blinkTimer.start();

  // run base pattern init logic
  SingleLedPattern::init();
//  m_blinkTimer.start(Time::getTickOffset(pos));
}

void BasicPattern::skip(uint32_t ticks)
{
  SingleLedPattern::skip(ticks);
  m_blinkTimer.start();
}

void BasicPattern::play()
{
  // check the alarm to toggle the light
  AlarmID id = m_blinkTimer.alarm();

  if (id == 0) {
    onBlinkOn();
    if (m_blinkTimer.onStart() && m_colorset.onStart()) {
      // callback for basic pattern started
      onBasicStart();
    }
  } else if (id == 1) {
    onBlinkOff();
    if (m_blinkTimer.onEnd() && m_colorset.onEnd()) {
      // callback for basic pattern ended
      onBasicEnd();
    }
  }
}

void BasicPattern::onBlinkOn()
{
  // set the target led with the given color
  Leds::setIndex(m_ledPos, m_colorset.getNext());
}

void BasicPattern::onBlinkOff()
{
  // clear the target led
  Leds::clearIndex(m_ledPos);
}

void BasicPattern::onBasicStart()
{
}

void BasicPattern::onBasicEnd()
{
}

void BasicPattern::resume()
{
  // start the blink timer from the curren time
  m_blinkTimer.start();
}

void BasicPattern::serialize(SerialBuffer &buffer) const
{
  SingleLedPattern::serialize(buffer);
  buffer.serialize(m_onDuration);
  buffer.serialize(m_offDuration);
}

void BasicPattern::unserialize(SerialBuffer &buffer)
{
}
