#include "TipTopPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

TipTopPattern::TipTopPattern() :
  MultiLedPattern(),
  m_blinkTimer1(),
  m_blinkTimer2()
{
}

TipTopPattern::~TipTopPattern()
{
}

// init the pattern to initial state
void TipTopPattern::init()
{
  MultiLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer1.reset();
  // dops timing
  m_blinkTimer1.addAlarm(2);
  m_blinkTimer1.addAlarm(13);
  // start the blink timer from the next frame
  m_blinkTimer1.start();

  // reset and add alarm
  m_blinkTimer2.reset();
  m_blinkTimer2.addAlarm(25);
  m_blinkTimer2.addAlarm(25);
  m_blinkTimer2.start();
}

// pure virtual must override the play function
void TipTopPattern::play()
{
  switch (m_blinkTimer1.alarm()) {
  case -1: // if blinktimer1 is off, just skip
    break;
  case 0:
    Leds::setAllTops(m_colorset.cur());
    m_colorset.getNext();
    break;
  case 1:
    Leds::clearAllTops();
    break;
  }
  switch (m_blinkTimer2.alarm()) {
  case -1:
    return;
  case 0:
    Leds::setAllTips(m_colorset.get(0));
    break;
  case 1:
    Leds::clearAllTips();
    break;
  }
}

// must override the serialize routine to save the pattern
void TipTopPattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void TipTopPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}
