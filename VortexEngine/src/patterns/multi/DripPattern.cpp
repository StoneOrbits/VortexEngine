#include "DripPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

DripPattern::DripPattern() :
  MultiLedPattern(),
  m_blinkTimer(),
  m_dripTimer(),
  m_sync(true)
{
}

DripPattern::~DripPattern()
{
}

// init the pattern to initial state
void DripPattern::init()
{
  MultiLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();
  // dops timing
  m_blinkTimer.addAlarm(10);
  m_blinkTimer.addAlarm(10);
  // start the blink timer from the next frame
  m_blinkTimer.start();

  // reset and add alarm
  m_dripTimer.reset();
  m_dripTimer.addAlarm(100);
  m_dripTimer.start();
}

// pure virtual must override the play function
void DripPattern::play()
{
  if (m_dripTimer.alarm() == 0) {
    m_sync = !m_sync;
    if (m_sync) {
      m_colorset.getNext();
    }
  }

  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // Turn on the leds
    if (!m_sync) {
      Leds::setAllTips(m_colorset.cur());
      Leds::setAllTops(m_colorset.peekNext());
    } else {
      Leds::setAll(m_colorset.cur());
    }
    break;
  case 1: // Turn off leds
    Leds::clearAll();
    break;
  }
}

// must override the serialize routine to save the pattern
void DripPattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void DripPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}