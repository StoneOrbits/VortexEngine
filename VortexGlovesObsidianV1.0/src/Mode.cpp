#include "Mode.h"

#include "patterns/Pattern.h"

Mode::Mode() :
  m_pPatterns(),
  m_pColorsets()
{
}

// bind a pattern and colorset to individual LED
bool Mode::bind(Pattern *pat, Colorset *set, LedPos pos)
{
  if (pos > LED_LAST) {
    return false;
  }
  m_pPatterns[pos] = pat;
  m_pColorsets[pos] = set;
  return true;
}

// bind a pattern and colorset to a range of LEDs
bool Mode::bindRange(Pattern *pat, Colorset *set, LedPos first, LedPos last)
{
  for (LedPos pos = first; pos <= last; ++pos) {
    if (!bind(pat, set, pos)) {
      return false;
    }
  }
  return true;
}

// bind a pattern and colorset to all LEDs
bool Mode::bindAll(Pattern *pat, Colorset *set)
{
  return bindRange(pat, set, LED_FIRST, LED_LAST);
}


bool Mode::setPattern(Pattern *pat, LedPos pos)
{
  if (pos > LED_LAST) {
    return false;
  }
  m_pPatterns[pos] = pat;
  return true;
}

bool Mode::setColorset(Colorset *set, LedPos pos)
{
  if (pos > LED_LAST) {
    return false;
  }
  m_pColorsets[pos] = set;
  return true;
}

Pattern *Mode::getPattern(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_pPatterns[pos];
}

Colorset *Mode::getColorset(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_pColorsets[pos];
}

void Mode::play()
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
    pattern->play(colorset, pos);

    // TODO: complex patterns
    // only run one finger of complex patterns
    //if (patter->isComplex()) {
    //  return;
    //}
  }
}
