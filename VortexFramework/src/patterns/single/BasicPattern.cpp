#include "BasicPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Colorset.h"
#include "../../Leds.h"
#include "../../Log.h"

BasicPattern::BasicPattern(uint8_t onDuration, uint8_t offDuration) :
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
  // run base pattern init logic
  SingleLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_onDuration);
  m_blinkTimer.addAlarm(m_offDuration);

  // start the blink timer from the next frame
  m_blinkTimer.start();
}

void BasicPattern::play()
{
  // check the alarm to toggle the light
  AlarmID id = m_blinkTimer.alarm();

  if (id == 0) {
    // when timer 0 runs out it's time to blink off
    onBlinkOff();
    if (m_blinkTimer.onEnd() && m_colorset.onEnd()) {
      // callback for basic pattern ended
      onBasicEnd();
    }
  } else if (id == 1) {
    // when timer 1 runs out it's time to blink on
    onBlinkOn();
    if (m_blinkTimer.onStart() && m_colorset.onStart()) {
      // callback for basic pattern started
      onBasicStart();
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

void BasicPattern::serialize(SerialBuffer &buffer) const
{
  //DEBUG_LOG("Serialize");
  SingleLedPattern::serialize(buffer);
  //buffer.serialize(m_onDuration);
  //buffer.serialize(m_offDuration);
}

void BasicPattern::unserialize(SerialBuffer &buffer)
{
  //DEBUG_LOG("Unserialize");
  SingleLedPattern::unserialize(buffer);
  //buffer.unserialize(&m_onDuration);
  //buffer.unserialize(&m_offDuration);
}
