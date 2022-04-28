#include "BasicPattern.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"

#include "../Log.h"

BasicPattern::BasicPattern(uint32_t onDuration, uint32_t offDuration) :
  m_onDuration(onDuration),
  m_blinkDuration(onDuration + offDuration),
  m_state(false)
{
}

BasicPattern::~BasicPattern()
{
}

void BasicPattern::play(Colorset *colorset, LedPos pos)
{
  if (!colorset) {
    // programmer error
    return;
  }

  // how far into a full frame this tick is
  uint32_t frameTime = Time::getCurtime(pos) % m_blinkDuration;

  // whether the light should be on based on curtime
  bool state = (frameTime < m_onDuration);

  // if the state hasn't changed then nothing to do
  if (state == m_state) {
    return;
  }

  // the state changed
  m_state = state;

  if (m_state) {
    // turn on with color
    Leds::setIndex(pos, colorset->getNext());
  } else {
    // turn off
    Leds::clearIndex(pos);
  }
}

void BasicPattern::serialize() const
{
  Pattern::serialize();
  Serial.print(m_onDuration);
  Serial.print(m_blinkDuration - m_onDuration);
}

void BasicPattern::unserialize()
{
}
