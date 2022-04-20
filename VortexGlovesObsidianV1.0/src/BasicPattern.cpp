#include "BasicPattern.h"
#include "LedControl.h"
#include "Time.h"

BasicPattern::BasicPattern(uint32_t onDuration, uint32_t offDuration) :
    m_onDuration(onDuration),
    m_offDuration(offDuration),
    m_blinkDuration(m_onDuration + m_offDuration),
    m_totalDuration(0),
    m_lightIsOn(false)
{
}

// custom bind for BasicPattern to calc total duration
bool BasicPattern::bind(const Colorset *colorset)
{
  if (!colorset || !super::bind(colorset)) {
    return false;
  }

  // each color in the colorset will be displayed on/off then the gap at end
  m_totalDuration = (colorset->numColors() * (m_onDuration + m_offDuration)) + m_gapDuration;
  
  return true;
}

// pure virtual must override the play function
void BasicPattern::play(LedControl *ledControl)
{
  if (!ledControl) {
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
    ledControl->clearAll(colorset->get(m_colorIndex));
    // increase the current color
    m_colorIndex = (m_colorIndex + 1) % colorset->numColors();
  } else {
    // turn off
    ledControl->clearAll()
  }
}
