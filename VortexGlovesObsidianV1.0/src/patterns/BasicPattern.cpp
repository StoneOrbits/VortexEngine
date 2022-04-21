#include "BasicPattern.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"

BasicPattern::BasicPattern(uint32_t onDuration, uint32_t offDuration) :
    m_onDuration(onDuration),
    m_blinkDuration(onDuration + offDuration),
    m_lightIsOn(false)
{
}

BasicPattern::~BasicPattern()
{
}

void BasicPattern::play(const TimeControl *timeControl, LedControl *ledControl, 
    Colorset *colorset, LedPos pos)
{
  if (!timeControl || !ledControl || !colorset) {
    // programmer error
    return;
  }

  // how far into a full frame this tick is
  uint32_t frameTime = timeControl->getCurtime(pos) % m_blinkDuration;

  // whether the light should be on based on curtime
  bool shouldBeOn = (frameTime <= m_onDuration);

  // if the state hasn't changed then nothing to do
  if (shouldBeOn == m_lightIsOn) {
    return;
  }

  // the state changed
  m_lightIsOn = shouldBeOn;

  if (m_lightIsOn) {
    // turn on with color
    ledControl->setIndex(pos, colorset->getNext());
  } else {
    // turn off
    ledControl->clearIndex(pos);
  }
}


