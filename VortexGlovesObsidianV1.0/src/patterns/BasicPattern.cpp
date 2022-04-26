#include "BasicPattern.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"

#include "../Log.h"

BasicPattern::BasicPattern(uint32_t onDuration, uint32_t offDuration) :
    m_onDuration(onDuration * TICK_PER_MS),
    m_blinkDuration((onDuration + offDuration) * TICK_PER_MS),
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
  uint32_t frameTime = g_pTimeControl->getCurtime(pos) % m_blinkDuration;

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
    g_pLedControl->setIndex(pos, colorset->getNext());
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
