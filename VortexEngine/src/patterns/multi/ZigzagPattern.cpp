#include "ZigzagPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"

ZigzagPattern::ZigzagPattern(uint8_t speed, uint8_t scale) :
  MultiLedPattern(),
  m_speed(speed),
  m_scale(scale)
{
}

ZigzagPattern::~ZigzagPattern()
{
}

// init the pattern to initial state
void ZigzagPattern::init()
{
  MultiLedPattern::init();
}

// pure virtual must override the play function
void ZigzagPattern::play()
{
  Leds::clearAll();
  LedPos target1 = (m_step < 5) ? fingerTop(Finger(m_step)) : fingerTip(Finger(LED_COUNT - m_step));
  Leds::setIndex(target1, m_colorset.cur());
  LedPos target2 = (m_step < 5) ? fingerTip(Finger((FINGER_COUNT - 1) - m_step)) : fingerTop(Finger(m_step - FINGER_COUNT));
  Leds::setIndex(target2, m_colorset.peekNext());
  m_step = (m_step + 1) % 10;
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
