#include "CrossDopsPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

CrossDopsPattern::CrossDopsPattern() :
  MultiLedPattern(),
  m_blinkTimer(),
  m_flipTimer(),
  m_ledMap(0)
{
}

CrossDopsPattern::~CrossDopsPattern()
{
}

// init the pattern to initial state
void CrossDopsPattern::init()
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
  m_flipTimer.reset();
  m_flipTimer.addAlarm(100);
  m_flipTimer.start();

  // Alternating tops and tips mapping of leds to turn on/off
  m_ledMap = MAP_LED(THUMB_TOP) |
             MAP_LED(INDEX_TIP) |
             MAP_LED(MIDDLE_TOP) |
             MAP_LED(RING_TIP) |
             MAP_LED(PINKIE_TOP);
}

// pure virtual must override the play function
void CrossDopsPattern::play()
{
  if (m_flipTimer.alarm() == 0) {
    m_colorset.getNext();
    // inverse of all the above tops/tips
    m_ledMap = MAP_INVERSE(m_ledMap);
  }
  
  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds for given mapping
    Leds::setMap(m_ledMap, m_colorset.cur());
    break;
  case 1: // turn off the leds
    Leds::clearAll();
    break;
  }
}

// must override the serialize routine to save the pattern
void CrossDopsPattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void CrossDopsPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}