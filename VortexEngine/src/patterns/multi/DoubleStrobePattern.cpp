#include "DoubleStrobePattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

DoubleStrobePattern::DoubleStrobePattern() :
  MultiLedPattern(),
  m_blinkTimer(),
  m_colorTimer()
{
}

DoubleStrobePattern::~DoubleStrobePattern()
{
}

// init the pattern to initial state
void DoubleStrobePattern::init()
{
  MultiLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();
  // dops timing
  m_blinkTimer.addAlarm(5);
  m_blinkTimer.addAlarm(8);
  // start the blink timer from the next frame
  m_blinkTimer.start();

  // reset and add alarm
  m_colorTimer.reset();
  m_colorTimer.addAlarm(75);
  m_colorTimer.start();
}

// pure virtual must override the play function
void DoubleStrobePattern::play()
{
  if (m_colorTimer.alarm() == 0) {
    m_colorset.getNext();
  }

  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on leds
    Leds::setAllTops(m_colorset.cur());
    Leds::setAllTips(m_colorset.peekNext());
    break;
  case 1: // turn off leds
    Leds::clearAll();
    break;
  }
}

// must override the serialize routine to save the pattern
void DoubleStrobePattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void DoubleStrobePattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}