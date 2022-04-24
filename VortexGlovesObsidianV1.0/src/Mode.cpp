#include "Mode.h"

#include "patterns/Pattern.h"

#include "Colorset.h"

Mode::Mode() :
  m_flags(MODE_FLAG_NONE),
  m_patternEntries()
{
}

Mode::~Mode()
{
  // TODO: better management, this is messy
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (m_patternEntries[pos].pattern) {
      delete m_patternEntries[pos].pattern;
    }
    if (m_patternEntries[pos].colorset) {
      delete m_patternEntries[pos].colorset;
    }
  }
}

// bind a pattern and colorset to individual LED
bool Mode::bind(Pattern *pat, Colorset *set, LedPos pos)
{
  if (pos > LED_LAST) {
    return false;
  }
  m_patternEntries[pos] = LedEntry(pat, set);
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
  if (m_patternEntries[pos].pattern) {
    delete m_patternEntries[pos].pattern;
  }
  m_patternEntries[pos].pattern = pat;
  return true;
}

bool Mode::setColorset(Colorset *set, LedPos pos)
{
  if (pos > LED_LAST) {
    return false;
  }
  if (m_patternEntries[pos].colorset) {
    delete m_patternEntries[pos].colorset;
  }
  m_patternEntries[pos].colorset = set;
  return true;
}

Pattern *Mode::getPattern(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_patternEntries[pos].pattern;
}

Colorset *Mode::getColorset(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_patternEntries[pos].colorset;
}

void Mode::play()
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // grab the entry for this led
    LedEntry entry = m_patternEntries[pos];
    if (!entry.pattern || !entry.colorset) {
      // incomplete pattern/set or empty slot
      continue;
    }
    // play the curren pattern with current color set on the current finger
    entry.pattern->play(entry.colorset, pos);
    // only run one pattern if this isn't a multi-pattern mode
    if (!hasFlags(MODE_FLAG_MULTI_PATTERN)) {
      return;
    }
  }
}
