#include "Mode.h"

#include "patterns/Pattern.h"

Mode::Mode() :
  m_pPatterns(),
  m_pColorsets()
{
}

// bind a pattern and colorset to individual LED
bool Mode::bind(LedPos pos, Pattern *pat, Colorset *set)
{
  if (pos > LED_LAST) {
    return false;
  }
  m_pPatterns[pos] = pat;
  m_pColorsets[pos] = set;
  return true;
}

// bind a pattern and colorset to a range of LEDs
bool Mode::bindRange(LedPos first, LedPos last, Pattern *pat, Colorset *set)
{
  for (LedPos pos = first; pos <= last; ++pos) {
    if (!bind(pos, pat, set)) {
      return false;
    }
  }
  return true;
}

// bind a pattern and colorset to all LEDs
bool Mode::bindAll(Pattern *pat, Colorset *set)
{
  return bindRange(LED_FIRST, LED_LAST, pat, set);
}

void Mode::play(const TimeControl *timeControl, LedControl *ledControl)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // An array of patterns, on for each LED
    Pattern *pattern = m_pPatterns[pos];
    // An array of Colorsets, one for each LED
    Colorset *colorset = m_pColorsets[pos];
    if (!pattern || !colorset) {
      // incomplete pattern/set or empty slot
      continue;
    }
    // play the curren pattern with current color set on the current finger
    pattern->play(timeControl, ledControl, colorset, pos);

    // TODO: complex patterns
    // only run one finger of complex patterns
    //if (patter->isComplex()) {
    //  return;
    //}
  }
}
