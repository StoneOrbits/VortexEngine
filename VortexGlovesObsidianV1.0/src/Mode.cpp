#include "Mode.h"

#include "patterns/SingleLedPattern.h"
#include "patterns/MultiLedPattern.h"
#include "patterns/Pattern.h"
#include "PatternBuilder.h"
#include "TimeControl.h"
#include "Colorset.h"

#include "Log.h"

#include <Arduino.h>

Mode::Mode() :
  m_flags(MODE_FLAG_NONE),
  m_ledEntries()
{
}

Mode::~Mode()
{
  unbindAll();
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

const Pattern *Mode::getPattern(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_ledEntries[pos].pattern;
}

const Colorset *Mode::getColorset(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_ledEntries[pos].colorset;
}

bool Mode::setPattern(PatternID pat, LedPos pos)
{
  // if it's a multi pattern ID then just set the multi pattern slot
  if (isMultiLedPatternID(pat)) {
    // clear any stored patterns first
    clearPatterns();
    MultiLedPattern *newPat = PatternBuilder::makeMulti(pat);
    if (!newPat) {
      return false;
    }
    m_multiPat = newPat;
    return true;
  }
  // if a specific LED was provided
  if (pos < LED_COUNT) {
    SingleLedPattern *newPat = PatternBuilder::makeSingle(pat);
    if (!newPat) {
      // failed to build new pattern, user gave multiled pattern id?
      return false;
    }
    m_ledEntries[pos].pattern = newPat;
    return true;
  }
  // otherwise iterate all of the LEDs and set single led patterns
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    SingleLedPattern *newPat = PatternBuilder::makeSingle(pat);
    if (!newPat) {
      // failed to build new pattern, user gave multiled pattern id?
      return false;
    }
    // update the pattern in this slot
    if (m_ledEntries[pos].pattern) {
      delete m_ledEntries[pos].pattern;
    }
    m_ledEntries[pos].pattern = newPat;
  }
  return true;
}

// this will in-place change the colorset for all 10x slots
bool Mode::setColorset(const Colorset *set, LedPos pos)
{
  if (pos != LED_COUNT) {
    if (m_ledEntries[pos].colorset) {
      *m_ledEntries[pos].colorset = *set;
      return true;
    }
  }
  // otherwise set all of the colorsets
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // if there is already a colorset just copy-initialize it
    if (m_ledEntries[pos].colorset) {
      *m_ledEntries[pos].colorset = *set;
      continue;
    }
    // otherwise create a new colorset in this slot
    m_ledEntries[pos].colorset = new Colorset(*set);
    if (!m_ledEntries[pos].colorset) {
      // error
    }
  }
  return true;
}

void Mode::clearPatterns()
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (!m_ledEntries[pos].pattern) {
      continue;
    }
    delete m_ledEntries[pos].pattern;
    m_ledEntries[pos].pattern = nullptr;
  }
}

void Mode::clearColorsets()
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (!m_ledEntries[pos].colorset) {
      continue;
    }
    delete m_ledEntries[pos].colorset;
    m_ledEntries[pos].colorset = nullptr;
  }
}
