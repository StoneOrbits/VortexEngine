#include "ZigzagPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

ZigzagPattern::ZigzagPattern(uint8_t speed, uint8_t scale) :
  MultiLedPattern(),
  m_speed(speed),
  m_scale(scale),
  m_step(FINGER_FIRST),
  m_blinkTimer(),
  m_stepTimer()
{
}

ZigzagPattern::~ZigzagPattern()
{
}

// init the pattern to initial state
void ZigzagPattern::init()
{
  MultiLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();
  // dops timing
  m_blinkTimer.addAlarm(2);
  m_blinkTimer.addAlarm(13);
  // start the blink timer from the next frame
  m_blinkTimer.start();

  // reset and add alarm
  m_stepTimer.reset();
  m_stepTimer.addAlarm(50);
  m_stepTimer.start();

  // start on the first color so that cur() works immediately
  m_colorset.setCurIndex(0);
}

// pure virtual must override the play function
void ZigzagPattern::play()
{
  // increment to the next step
  m_step = (Finger)((m_step + 1) % 10);
  if (m_step == 3 || m_step == 8) {
    m_colorset.getNext();
  }

  int alm = m_blinkTimer.alarm();
  // if first alarm is not triggering
  if (alm == -1) {
    // just return
    return;
  }
  // otherwise first alarm is triggering

  // determine two target leds
  LedPos target1 = (m_step < 5) ? fingerTip(m_step) : fingerTop((Finger)((LED_COUNT - m_step) - 1));
  LedPos target2 = (m_step < 5) ? fingerTop((Finger)((5 - m_step) - 1)) : fingerTip((Finger)((m_step - 5)));

  // if starting the 'off' phase then turn the leds off
  if (alm == 1) {
    Leds::clearAll();
    return;
  }

  // otherwise alarm is 0, starting the 'on' phase'
  // turn on target leds with current color
  Leds::setIndex(target1, m_colorset.cur());
  Leds::setIndex(target2, m_colorset.peekNext());
}

// must override the serialize routine to save the pattern
void ZigzagPattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
  buffer.serialize(m_speed);
  buffer.serialize(m_scale);
}

void ZigzagPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
  buffer.unserialize(&m_speed);
  buffer.unserialize(&m_scale);
}
