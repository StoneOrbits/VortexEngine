#include "DemoPattern.h"

DemoPattern::DemoPattern() :
    m_tracerDuration(20),
    m_totalDuration(m_tracerDuration + 3),
    m_tracerCounter(0)
{
}

// pure virtual must override the play function
void DemoPattern::play(Colorset *colorset, LedPos pos)
{
  if (!colorset) {
    // programmer error
    return;
  }

  // how far into a full frame this tick is
  uint32_t frameTime = g_pTimeControl->getCurtime(pos) % m_totalDuration;

  // 20(0) 3(n) 20(0) 3(n+1)
  if (frameTime <= m_tracerDuration) {
    g_pLedControl->setIndex(pos, colorset->get(0));
  } else {
    // set the color of the tracer counter
    g_pLedControl->setIndex(pos, colorset->get(1 + m_tracerCounter));
    // increment tracer counter and wrap at 1 less than num colors
    m_tracerCounter = (m_tracercounter + 1) % (colorset->numColors() - 1);
  }
}

// must override the serialize routine to save the pattern
void DemoPattern::serialize() const
{
}

// must override unserialize to load patterns
void DemoPattern::unserialize()
{
}
