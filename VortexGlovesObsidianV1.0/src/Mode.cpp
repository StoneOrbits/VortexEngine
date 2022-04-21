#include "Mode.h"
#include "patterns/Pattern.h"

Mode::Mode() :
  m_pPattern(nullptr),
  m_pColorset(nullptr)
{
}

Mode::Mode(Pattern *pat, Colorset *set) :
  m_pPattern(pat),
  m_pColorset(set)
{
}

void Mode::play(const TimeControl *timeControl, LedControl *ledControl)
{
  if (!m_pPattern) {
    return;
  }
  for (LedPos pos = THUMB_TOP; pos < NUM_LEDS; ++pos) {
    // An array of patterns, on for each LED
    Pattern *pat = m_pPattern[pos]
    // An array of Colorsets, one for each LED
    Colorset *set = m_pColorset[pos];
    if (!pat || !set) {
      // incomplete pattern/set or empty slot
      continue;
    }
    // play the curren pattern with current color set
    m_pPattern->play(timeControl, ledControl, m_pColorset, pos);
  }
}
