#include "BasicPattern.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Log.h"

BasicPattern::BasicPattern(uint32_t onDuration, uint32_t offDuration) :
  m_onDuration(onDuration),
  m_offDuration(offDuration),
  m_blinkTimer()
{
}

BasicPattern::~BasicPattern()
{
}

void BasicPattern::init(Colorset *colorset, LedPos pos)
{
  // run base pattern init logic
  Pattern::init(colorset, pos);

  // initialize the colorset to start on the right color
  uint32_t totalTime = m_onDuration + m_offDuration;
  if (Time::getTickOffset(pos) > totalTime) {
    m_pColorset->skip();
  }

  m_blinkTimer.reset();
  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_onDuration);
  m_blinkTimer.addAlarm(m_offDuration);
  // start the timer with current tick offset
  m_blinkTimer.start(Time::getTickOffset(pos));
}

void BasicPattern::play()
{
  // if alarm returns an alarm ID then it will start watching for
  // the next alarm so you cannot call it twice in one tick otherwise
  // you will get a different result
  int32_t numTriggers = 0;
  AlarmID id = m_blinkTimer.alarm(&numTriggers);

  // if the alarm has triggered more than once then skip that many colors
  // in the colorset so that it lines up. This will happen if the time offset
  // for the finger causes it to start 2x or 3x after the trigger time
  if (numTriggers != 0) {
    m_pColorset->skip(numTriggers);
  }

  // switch on the alarm result
  switch (id) {
  case ALARM_NONE:
  default: // the alarm did not trigger
    return;
  case 0: // the first alarm triggered, turn on with color
    Leds::setIndex(m_ledPos, m_pColorset->getNext());
    break;
  case 1: // the second alarm triggered, turn off with color
    Leds::clearIndex(m_ledPos);
    break;
  }
}

void BasicPattern::serialize() const
{
  Pattern::serialize();
  Serial.print(m_onDuration);
  Serial.print(m_offDuration);
}

void BasicPattern::unserialize()
{
}
