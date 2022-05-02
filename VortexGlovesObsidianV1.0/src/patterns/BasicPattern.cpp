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

  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_onDuration);
  m_blinkTimer.addAlarm(m_offDuration);

  // start the blink timer from the current frame
  m_blinkTimer.start();

  // skip forward however many ticks this led is offset
  skip(Time::getTickOffset(m_ledPos));
  // need to start the blink timer again because it got shifted
  m_blinkTimer.start();
}

void BasicPattern::play()
{
  AlarmID id = m_blinkTimer.alarm();
  if (id == 0)      Leds::setIndex(m_ledPos, m_pColorset->getNext());
  else if (id == 1) Leds::clearIndex(m_ledPos);
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
