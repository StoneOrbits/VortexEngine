#include "PulsishPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

PulsishPattern::PulsishPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t onDuration2, uint8_t offDuration2, uint8_t stepDuration) :
  MultiLedPattern(),
  m_progress(),
  m_stepDuration(stepDuration),
  m_stepTimer(),
  m_onDuration1(onDuration1),
  m_offDuration1(offDuration1),
  m_onDuration2(onDuration2),
  m_offDuration2(offDuration2),
  m_blinkTimer(),
  m_blink2Timer()
{
}

PulsishPattern::~PulsishPattern()
{
}

// init the pattern to initial state
void PulsishPattern::init()
{
  MultiLedPattern::init();

  // reset and add alarm
  m_stepTimer.reset();
  m_stepTimer.addAlarm(m_stepDuration);
  m_stepTimer.start();

  // make blink timer
  m_blinkTimer.reset();
  m_blinkTimer.addAlarm(m_onDuration1);
  m_blinkTimer.addAlarm(m_offDuration1);
  m_blinkTimer.start();

  // make 2nd blink timer
  m_blink2Timer.reset();
  m_blink2Timer.addAlarm(m_onDuration2);
  m_blink2Timer.addAlarm(m_offDuration2);
  m_blink2Timer.start();
}
// pure virtual must override the play function
void PulsishPattern::play()
{
  // when the step timer triggers
  if (m_stepTimer.alarm() == 0) {
    m_progress = (m_progress + 1) % FINGER_COUNT;
  }

  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    for (Finger finger = FINGER_FIRST; finger < FINGER_COUNT; ++finger) {
      if (finger != m_progress) {
        Leds::setFinger(finger, m_colorset.cur());        
      }
    }
    m_colorset.skip();
    if (m_colorset.curIndex() == 0) {
      m_colorset.skip();
    }
    break;
  case 1:
    for (Finger finger = FINGER_FIRST; finger < FINGER_COUNT; ++finger) {
      if (finger != m_progress) {
        Leds::clearFinger(finger);
      }
    }
    break;
  }

  switch (m_blink2Timer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    Leds::setFinger((Finger)m_progress, m_colorset.get(0));
    break;
  case 1:
    Leds::clearFinger((Finger)m_progress);
    break;
  }
}

// must override the serialize routine to save the pattern
void PulsishPattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
  buffer.serialize(m_onDuration1);
  buffer.serialize(m_offDuration1);
  buffer.serialize(m_onDuration2);
  buffer.serialize(m_offDuration2);
  buffer.serialize(m_stepDuration);
}

void PulsishPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
  buffer.unserialize(&m_onDuration1);
  buffer.unserialize(&m_offDuration1);
  buffer.unserialize(&m_onDuration2);
  buffer.unserialize(&m_offDuration2);
  buffer.unserialize(&m_stepDuration);
}

#ifdef TEST_FRAMEWORK
void PulsishPattern::saveTemplate(int level) const
{
  MultiLedPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"OnDuration1\": %d,", m_onDuration1);
  IndentMsg(level + 1, "\"OffDuration1\": %d,", m_offDuration1);
  IndentMsg(level + 1, "\"OnDuration2\": %d,", m_onDuration2);
  IndentMsg(level + 1, "\"OffDuration2\": %d,", m_offDuration2);
  IndentMsg(level + 1, "\"StepDuration\": %d,", m_stepDuration);
}
#endif

