#include "WarpWormPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

WarpWormPattern::WarpWormPattern(uint8_t stepDuration, uint8_t snakeSize, uint8_t fadeAmount) :
  MultiLedPattern(),
  m_stepTimer(),
  m_blinkTimer(),
  m_progress()
{
}

WarpWormPattern::~WarpWormPattern()
{
}

// init the pattern to initial state
void WarpWormPattern::init()
{
  MultiLedPattern::init();

  m_blinkTimer.reset();
  m_blinkTimer.addAlarm(2);
  m_blinkTimer.addAlarm(7);
  m_blinkTimer.start();

  // reset and add alarm
  m_stepTimer.reset();
  m_stepTimer.addAlarm(100);
  m_stepTimer.start();

  m_colorset.setCurIndex(1);

}

// pure virtual must override the play function
void WarpWormPattern::play()
{
  // when the step timer triggers
  if (m_stepTimer.alarm() == 0) {
    m_progress = (m_progress + 1) % LED_COUNT;
    if (m_progress == 0) {
      m_colorset.getNext();
      if (m_colorset.curIndex() == 0) {
        m_colorset.getNext();
      }
    }
  }

  int wormSize = 6;
  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    Leds::setAll(m_colorset.get(0));
    for (int body = 0; body < wormSize; ++body) {
      if (body + m_progress < LED_COUNT) {
        Leds::setIndex((LedPos)(body + m_progress), m_colorset.cur());
      } else {
        RGBColor col = m_colorset.peekNext();
        if (m_colorset.curIndex() == m_colorset.numColors() - 1) {
          col = m_colorset.peek(2);
        }
        Leds::setIndex((LedPos)((body + m_progress) % LED_COUNT), col);
      }
    }
    break;
  case 1:
    Leds::clearAll();
    break;
  }
}

// must override the serialize routine to save the pattern
void WarpWormPattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void WarpWormPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}