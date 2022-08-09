#include "VortexWipePattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

const LedPos VortexWipePattern::ledStepPositions[] = {
  THUMB_TOP,
  INDEX_TOP,
  MIDDLE_TOP,
  RING_TOP,
  PINKIE_TOP,

  PINKIE_TIP,
  RING_TIP,
  MIDDLE_TIP,
  INDEX_TIP,
  THUMB_TIP
};

VortexWipePattern::VortexWipePattern(uint8_t stepDuration, uint8_t snakeSize, uint8_t fadeAmount) :
  MultiLedPattern(),
  m_stepTimer(),
  m_blinkTimer(),
  m_progress()
{
}

VortexWipePattern::~VortexWipePattern()
{
}

// init the pattern to initial state
void VortexWipePattern::init()
{
  MultiLedPattern::init();

  m_blinkTimer.reset();
  m_blinkTimer.addAlarm(2);
  m_blinkTimer.addAlarm(7);
  m_blinkTimer.start();

  // reset and add alarm
  m_stepTimer.reset();
  m_stepTimer.addAlarm(125);
  m_stepTimer.start();

  m_colorset.setCurIndex(0);

}

// pure virtual must override the play function
void VortexWipePattern::play()
{
  // when the step timer triggers
  if (m_stepTimer.alarm() == 0) {
    m_progress = (m_progress + 1) % LED_COUNT;
    if (m_progress == 0) {
      m_colorset.getNext();
    }
  }

  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    for (int index = 0; index < m_progress; ++index) {
      Leds::setIndex(ledStepPositions[index], m_colorset.peekNext());
    }
    for (int index = m_progress; index < LED_COUNT; ++index) {
      Leds::setIndex(ledStepPositions[index], m_colorset.cur());
    } 
    break;
  case 1:
    Leds::clearAll();
    break;
  }
}

// must override the serialize routine to save the pattern
void VortexWipePattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void VortexWipePattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}