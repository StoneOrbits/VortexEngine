#include "SparkleTracePattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

SparkleTracePattern::SparkleTracePattern() :
  MultiLedPattern(),
  m_blinkTimer(),
  m_spawnTimer()
{
}

SparkleTracePattern::~SparkleTracePattern()
{
}

// init the pattern to initial state
void SparkleTracePattern::init()
{
  MultiLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();
  // dops timing
  m_blinkTimer.addAlarm(5);
  // start the blink timer from the next frame
  m_blinkTimer.start();

  // reset and add alarm
  m_spawnTimer.reset();
  m_spawnTimer.addAlarm(50);
  m_spawnTimer.start();
}

// pure virtual must override the play function
void SparkleTracePattern::play()
{
  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    Leds::setAll(m_colorset.get(0));
    break;
  }

  if (m_spawnTimer.alarm() == 0) {
    for (int dot = 0; dot < 4; ++dot) {
    Leds::setFinger((Finger)random(FINGER_FIRST, FINGER_LAST + 1), m_colorset.cur());
    }
    m_colorset.getNext();
    if (m_colorset.curIndex() == 0)
      m_colorset.getNext();
  }
}

// must override the serialize routine to save the pattern
void SparkleTracePattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void SparkleTracePattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}