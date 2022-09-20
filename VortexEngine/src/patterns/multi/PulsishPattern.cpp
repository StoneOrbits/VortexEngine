#include "PulsishPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"
#include "../../Timings.h"

PulsishPattern::PulsishPattern(uint8_t stepDuration, uint8_t snakeSize, uint8_t fadeAmount) :
  MultiLedPattern(),
  m_progress(),
  m_stepTimer(),
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
  m_stepTimer.addAlarm(100);
  m_stepTimer.start();

  // make blink timer
  m_blinkTimer.reset();
  m_blinkTimer.addAlarm(DOPISH_ON_DURATION);
  m_blinkTimer.addAlarm(DOPISH_OFF_DURATION);
  m_blinkTimer.start();

  // make 2nd blink timer
  m_blink2Timer.reset();
  m_blink2Timer.addAlarm(STROBE_ON_DURATION);
  m_blink2Timer.addAlarm(STROBE_OFF_DURATION);
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
}

void PulsishPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}