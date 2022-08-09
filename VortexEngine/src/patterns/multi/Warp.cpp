#include "WarpPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

WarpPattern::WarpPattern(uint8_t stepDuration, uint8_t snakeSize, uint8_t fadeAmount) :
  MultiLedPattern(),
  m_stepTimer(),
  m_blinkTimer(),
  m_progress()
{
}

WarpPattern::~WarpPattern()
{
}

// init the pattern to initial state
void WarpPattern::init()
{
  MultiLedPattern::init();

  m_blinkTimer.reset();
  m_blinkTimer.addAlarm(3);
  m_blinkTimer.addAlarm(12);
  m_blinkTimer.start();

  // reset and add alarm
  m_stepTimer.reset();
  m_stepTimer.addAlarm(50);
  m_stepTimer.start();

  m_colorset.setCurIndex(0);

}

// pure virtual must override the play function
void WarpPattern::play()
{
  // when the step timer triggers
  if (m_stepTimer.alarm() == 0) {
    m_progress = (m_progress + 1) % FINGER_COUNT;
    if (m_progress == 0) {
      m_colorset.getNext();
    }
  }

  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    Leds::setAll(m_colorset.cur());
    Leds::setFinger((Finger)m_progress, m_colorset.peekNext());
    break;
  case 1:
    Leds::clearAll();
    break;
  }
}

// must override the serialize routine to save the pattern
void WarpPattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void WarpPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}