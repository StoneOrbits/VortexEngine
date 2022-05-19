#include "Mode.h"

#include "patterns/SingleLedPattern.h"
#include "patterns/MultiLedPattern.h"
#include "patterns/Pattern.h"
#include "PatternBuilder.h"
#include "TimeControl.h"
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
    if (!hasFlags(MODE_FLAG_MULTI_LED)) {
      break;
    }
  }
}

void Mode::init()
{
#if 0
  if (m_isComplex) {
    if (m_ledEntries[0].colorset) {
      m_ledEntries[0].colorset->init();
    }
    if (m_ledEntries[0].pattern) {
      // complex pattern init
      m_ledEntries[0].pattern->init(m_ledEntries[0].colorset, LED_COUNT);
    }
    return;
  }
#endif
  // otherwise regular init
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // grab the entry for this led
    LedEntry entry = m_ledEntries[pos];
    if (entry.colorset) {
      entry.colorset->init();
    }
    if (entry.pattern) {
      // the pattern may change the colorset initial position
      entry.pattern->init(entry.colorset, pos);
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
    entry.pattern->play();
    // only run one pattern if this isn't a multi-pattern mode
    if (!hasFlags(MODE_FLAG_MULTI_LED)) {
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
    if (!hasFlags(MODE_FLAG_MULTI_LED)) {
      return;
    }
  }
}

void Mode::unserialize()
{
}

// bind a single led pattern and colorset to individual LED
bool Mode::bindSingle(SingleLedPattern *pat, Colorset *set, LedPos pos)
{
  // don't allow binding single on invalid LED, or a mode that already has multi
  if (pos > LED_LAST || hasFlags(MODE_FLAG_MULTI_LED)) {
    return false;
  }
  unbindSingle(pos);
  m_ledEntries[pos].pattern = pat;
  m_ledEntries[pos].colorset = set;
  return true;
}

// bind a multi led pattern and colorset to all LEDs
bool Mode::bindMulti(MultiLedPattern *pat, Colorset *set)
{
  unbindAll();
  m_multiPat = pat;
  m_multiColorset = set;
  addFlags(MODE_FLAG_MULTI_LED);
  return true;
}

void Mode::unbindSingle(LedPos pos)
{
  if (pos > LED_LAST || hasFlags(MODE_FLAG_MULTI_LED)) {
    return;
  }
  if (m_ledEntries[pos].pattern) {
    delete m_ledEntries[pos].pattern;
    m_ledEntries[pos].pattern = nullptr;
  }
  if (m_ledEntries[pos].colorset) {
    delete m_ledEntries[pos].colorset;
    m_ledEntries[pos].colorset = nullptr;
  }
}

void Mode::unbindMulti()
{
  if (m_multiPat) {
    delete m_multiPat;
    m_multiPat = nullptr;
  }
  if (m_multiColorset) {
    delete m_multiColorset;
    m_multiColorset = nullptr;
  }
  // no longer a multi-led mode
  clearFlags(MODE_FLAG_MULTI_LED);
}

void Mode::unbindAll()
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    unbindSingle(pos);
  }
}

SingleLedPattern *Mode::getSinglePattern(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_ledEntries[pos].pattern;
}

Colorset *Mode::getSingleColorset(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_ledEntries[pos].colorset;
}

MultiLedPattern *Mode::getMultiPattern() const
{
  return m_multiPat;
}

Colorset *Mode::getMultiColorset() const
{
  return m_multiColorset;
}

// this will in-place change the single led pattern for all 10x slots
bool Mode::changePattern(const SingleLedPattern *pat, LedPos pos)
{
  if (!pat || pos > LED_LAST) {
    // programmer error
    return false;
  }
  SingleLedPattern *newPat = PatternBuilder::makeSingle(pat->getPatternID());
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
bool Mode::changeAllPatterns(const SingleLedPattern *pat)
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

