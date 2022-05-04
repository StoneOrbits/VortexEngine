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
  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_onDuration);
  m_blinkTimer.addAlarm(m_offDuration);

  // start the blink timer from the current frame
  m_blinkTimer.start();

  // run base pattern init logic
  Pattern::init(colorset, pos);
}

void BasicPattern::play()
{
  AlarmID id = m_blinkTimer.alarm();
  if (id == 0) {
    Leds::setIndex(m_ledPos, m_pColorset->getNext());
  } else if (id == 1) {
    Leds::clearIndex(m_ledPos);
  }
}

bool BasicPattern::onEnd() const
{
  // basic sanity check
  if (!m_pColorset || !m_pColorset->numColors()) {
    return false;
  }
  // make sure the color is on the very last color
  if (m_pColorset->curIndex() != (m_pColorset->numColors() - 1)) {
    return false;
  }
  // is the blink timer on it's end frame
  if (!m_blinkTimer.onEnd()) {
    return false;
  }
  // must be the end frame of the pattern
  return true;
}

void BasicPattern::resume()
{
  // start the blink timer from the curren time
  m_blinkTimer.start();
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
