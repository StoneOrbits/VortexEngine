#include "BasicPattern.h"
#include "LedControl.h"
#include "Colorset.h"
#include "Time.h"

BasicPattern::BasicPattern(uint32_t onDuration, uint32_t offDuration) :
    m_onDuration(onDuration),
    m_offDuration(offDuration),
    m_blinkDuration(m_onDuration + m_offDuration),
    m_colorIndex(0),
    m_lightIsOn(false)
{
}

// pure virtual must override the play function
void BasicPattern::play(LedControl *ledControl, Colorset *colorset)
{
  if (!ledControl || !colorset) {
    // programmer error
    return;
  }

  // how far into a full frame this tick is
  uint32_t frameTime = g_curTime % m_blinkDuration;

  // whether the light should be on based on curtime
  bool shouldBeOn = (frameTime <= m_onDuration);

  // if the state hasn't changed then nothing to do
  if (shouldBeOn == m_lightIsOn) {
    return;
  }

  // the state changed
  m_lightIsOn = shouldBeOn;

  if (!m_lightIsOn) {
    // turn on with color
    ledControl->clearAll(colorset->getNext());
  } else {
    // turn off
    ledControl->clearAll();
  }
}
