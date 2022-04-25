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

void BasicPattern::play(Colorset *colorset, LedPos pos)
{
  if (!colorset) {
    // programmer error
    return;
  }

  // how far into a full frame this tick is
  uint32_t frameTime = g_pTimeControl->getCurtime(pos) % m_blinkDuration;

  // whether the light should be on based on curtime
  bool shouldBeOn = (frameTime <= m_onDuration);

  // if the state hasn't changed then nothing to do
  if (shouldBeOn != m_lightIsOn) {
    if (m_lightIsOn) {
        colorset->getNext();
    }
  }

  // the state changed
  m_lightIsOn = shouldBeOn;

  if (m_lightIsOn) {
    // turn on with color
    g_pLedControl->setIndex(pos, colorset->cur());
  } else {
    // turn off
    g_pLedControl->clearIndex(pos);
  }
}

void BasicPattern::serialize() const
{
}

void BasicPattern::unserialize()
{
}
