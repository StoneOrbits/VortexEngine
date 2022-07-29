#include "ZigzagPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"

ZigzagPattern::ZigzagPattern(uint8_t speed, uint8_t scale) :
  MultiLedPattern(),
  m_speed(speed),
  m_scale(scale),
  m_blinkTimer()
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

  // start on the first color so that cur() works immediately
  m_colorset.setCurIndex(0);
}

// pure virtual must override the play function
void ZigzagPattern::play()
{
  // clear all the leds
  Leds::clearAll();

  // if first alarm is not triggering
  if (m_blinkTimer.alarm() != 0) {
    // just return
    return;
  }
  // otherwise first alarm is triggering

  // determine two target leds
  LedPos target1 = (m_step < 5) ? fingerTop((Finger)m_step) : fingerTip((Finger)(LED_COUNT - m_step));
  LedPos target2 = (m_step < 5) ? fingerTip((Finger)((FINGER_COUNT - 1) - m_step)) : fingerTop((Finger)(m_step - FINGER_COUNT));

  // turn on target leds with current color
  Leds::setIndex(target1, m_colorset.cur());
  Leds::setIndex(target2, m_colorset.peekNext());

  // every 10 steps move to the next color
  m_step = (m_step + 1) % 9;
  if (!m_step) {
    m_colorset.getNext();
  }
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
