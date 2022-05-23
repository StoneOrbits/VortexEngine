#include "Mode.h"

#include "patterns/SingleLedPattern.h"
#include "patterns/MultiLedPattern.h"
#include "patterns/Pattern.h"
#include "PatternBuilder.h"
#include "SerialBuffer.h"
#include "TimeControl.h"
#include "Colorset.h"
#include "Log.h"

#include <Arduino.h>

using namespace std;

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

void Mode::serialize(SerialBuffer &buffer) const
{
  //   4 mode flags (*)       flags defined whether multi pattern or not
  //     led1..N {            if multi pattern then 10x led, otherwise 1x
  //      1 led (0 - 9)
  //      1 pattern id (0 - 255)
  //      colorset1..N {
  //       1 numColors (0 - 255)
  //       rgb1..N {
  //        1 red (0-255)
  //        1 grn (0-255)
  //        1 blu (0-255)
  //       }
  //      }
  buffer.serialize((uint32_t)m_flags);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    const LedEntry *entry = m_ledEntries + pos;
    if (!entry->pattern || !entry->colorset) {
      continue;
    }

    // just serialize the pattern then colorset
    entry->pattern->serialize(buffer);
    entry->colorset->serialize(buffer);

    // if the multi pattern flag is present only write one entry
    if (hasFlags(MODE_FLAG_MULTI_LED)) {
      return;
    }
  }
}

void Mode::unserialize(SerialBuffer &buffer)
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

PatternID Mode::getPatternID(LedPos pos) const
{
  if (pos > LED_LAST) {
    return PATTERN_FIRST;
  }
  return getPattern()->getPatternID();
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
    // initialize the new pattern
    m_multiPat->init(m_ledEntries[pos].colorset, pos);
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
    // initialize the new pattern
    m_ledEntries[pos].pattern->init(m_ledEntries[pos].colorset, pos);
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
    // initialize the new pattern
    m_ledEntries[pos].pattern->init(m_ledEntries[pos].colorset, pos);
  }
  return true;
}

// this will in-place change the colorset for all 10x slots
bool Mode::setColorset(const Colorset *set, LedPos pos)
{
  if (pos != LED_COUNT) {
    if (m_ledEntries[pos].colorset) {
      *m_ledEntries[pos].colorset = *set;
      m_ledEntries[pos].pattern->init(m_ledEntries[pos].colorset, pos);
      return true;
    }
  }
  // otherwise set all of the colorsets
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // if there is already a colorset just copy-initialize it
    if (m_ledEntries[pos].colorset) {
      *m_ledEntries[pos].colorset = *set;
      // re-initialize the pattern with the new colorset
      m_ledEntries[pos].pattern->init(m_ledEntries[pos].colorset, pos);
      continue;
    }
    // otherwise create a new colorset in this slot
    m_ledEntries[pos].colorset = new Colorset(*set);
    if (!m_ledEntries[pos].colorset) {
      ERROR_OUT_OF_MEMORY();
    }
    // re-initialize the pattern with the new colorset
    m_ledEntries[pos].pattern->init(m_ledEntries[pos].colorset, pos);
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
