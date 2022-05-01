#include "Mode.h"

#include "patterns/Pattern.h"
#include "PatternBuilder.h"
#include "Colorset.h"

#include <Arduino.h>

Mode::Mode() :
  m_flags(MODE_FLAG_NONE),
  m_ledEntries()
{
}

Mode::~Mode()
{
  // TODO: better management, this is messy
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (m_ledEntries[pos].pattern) {
      delete m_ledEntries[pos].pattern;
    }
    if (m_ledEntries[pos].colorset) {
      delete m_ledEntries[pos].colorset;
    }
    // only delete the first one
    if (!hasFlags(MODE_FLAG_MULTI_PATTERN)) {
      break;
    }
  }
}

void Mode::init()
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // grab the entry for this led
    LedEntry entry = m_ledEntries[pos];
    // reset the colorset index counter and the pattern timers
    if (entry.pattern) {
      entry.pattern->init(pos);
    }
    if (entry.colorset) {
      entry.colorset->init();
    }
  }
}

void Mode::play()
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // grab the entry for this led
    LedEntry entry = m_ledEntries[pos];
    if (!entry.pattern || !entry.colorset) {
      // incomplete pattern/set or empty slot
      continue;
    }
    // play the curren pattern with current color set on the current finger
    entry.pattern->play(entry.colorset);
    // only run one pattern if this isn't a multi-pattern mode
    if (!hasFlags(MODE_FLAG_MULTI_PATTERN)) {
      //return;
    }
  }
}

void Mode::serialize() const
{
  //   4 mode flags (*)       flags defined whether multi pattern or not
  //     led1..N {            if multi pattern then 10x led, otherwise 1x
  //      1 led (0 - 9)
  //      1 pattern id (0 - 255)
  //      colorset1..N {
  //       1 numColors (0 - 255)
  //       hsv1..N {
  //        1 hue (0-255)
  //        1 sat (0-255)
  //        1 val (0-255)
  //       }
  //      }
  Serial.print(m_flags);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    const LedEntry *entry = m_ledEntries + pos;
    if (!entry->pattern || !entry->colorset) {
      continue;
    }

    // just serialize the pattern then colorset
    entry->pattern->serialize();
    entry->colorset->serialize();

    // if the multi pattern flag isn't present onlyG write one entry
    if (!hasFlags(MODE_FLAG_MULTI_PATTERN)) {
      return;
    }
  }
}

void Mode::unserialize()
{
}

// bind a pattern and colorset to individual LED
bool Mode::bind(Pattern *pat, Colorset *set, LedPos pos)
{
  if (pos > LED_LAST) {
    return false;
  }
  if (m_ledEntries[pos].pattern) {
    delete m_ledEntries[pos].pattern;
  }
  if (m_ledEntries[pos].colorset) {
    delete m_ledEntries[pos].colorset;
  }
  m_ledEntries[pos].pattern = pat;
  m_ledEntries[pos].colorset = set;
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

Pattern *Mode::getPattern(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_ledEntries[pos].pattern;
}

Colorset *Mode::getColorset(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_ledEntries[pos].colorset;
}

// this will in-place change the pattern for all 10x slots
bool Mode::changePattern(const Pattern *pat, LedPos pos)
{
  if (!pat || pos > LED_LAST) {
    // programmer error
    return false;
  }
  Pattern *newPat = PatternBuilder::make(pat->getPatternID());
  if (!newPat) {
    // failed to build new pattern
    return false;
  }
  if (m_ledEntries[pos].pattern) {
    delete m_ledEntries[pos].pattern;
  }
  m_ledEntries[pos].pattern = newPat;
  return true;
}

// this will in-place change the colorset for all 10x slots
bool Mode::changeColorset(const Colorset *set, LedPos pos)
{
  if (!set || pos > LED_LAST) {
    // programmer error
    return false;
  }
  Colorset *newSet = new Colorset(*set);
  if (!newSet) {
    // failed to build new colorset
    return false;
  }
  if (m_ledEntries[pos].colorset) {
    delete m_ledEntries[pos].colorset;
  }
  m_ledEntries[pos].colorset = newSet;
  return true;
}

// this will in-place change the pattern for all 10x slots
bool Mode::changeAllPatterns(const Pattern *pat)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (!changePattern(pat, pos)) {
      return false;
    }
  }
  return true;
}

// this will in-place change the colorset for all 10x slots
bool Mode::changeAllColorsets(const Colorset *set)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (!changeColorset(set, pos)) {
      return false;
    }
  }
  return true;
}

