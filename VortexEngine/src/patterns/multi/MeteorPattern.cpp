#include "MeteorPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

MeteorPattern::MeteorPattern() :
  MultiLedPattern(),
  m_blinkTimer(),
  m_spawnTimer(),
  m_stash()
{
}

MeteorPattern::~MeteorPattern()
{
}

// init the pattern to initial state
void MeteorPattern::init()
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
  m_spawnTimer.reset();
  m_spawnTimer.addAlarm(50);
  m_spawnTimer.start();
}

// pure virtual must override the play function
void MeteorPattern::play()
{
  if (m_spawnTimer.alarm() == 0) {
    Leds::setFinger((Finger)random(FINGER_FIRST, FINGER_LAST + 1), m_colorset.getNext());
  }

  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    Leds::restoreAll(m_stash);
    Leds::adjustBrightnessAll(15);
    break;
  case 1: // turn on the leds
    Leds::stashAll(m_stash);
    Leds::clearAll();
    break;
  }
}

// must override the serialize routine to save the pattern
void MeteorPattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void MeteorPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}