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
  // otherwise regular init
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // grab the entry for this led and initialize it
    Pattern *entry = m_ledEntries[pos];
    if (!entry) {
      continue;
    }
    entry->init();
  }
}

void Mode::play()
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // grab the entry for this led
    Pattern *entry = m_ledEntries[pos];
    if (!entry) {
      // incomplete pattern/set or empty slot
      continue;
    }
    // play the curren pattern with current color set on the current finger
    entry->play();
    // if either of these flags are present only play the first pattern
    if (hasFlags(MODE_FLAG_MULTI_LED)) {
      //break;
    }
  }
}

void Mode::serialize(SerialBuffer &buffer) const
{
  DEBUG("Serialize");
  //   4 mode flags (*)       flags defined whether multi pattern or not
  //     led1..N {            if multi pattern then 10x led, otherwise 1x
  //      colorset1..N {
  //       1 numColors (0 - 255)
  //       rgb1..N {
  //        1 red (0-255)
  //        1 grn (0-255)
  //        1 blu (0-255)
  //       }
  //      }
  //      1 pattern id (0 - 255)
  //
  DEBUGF("Saved mode flags: %x (%u %u)", m_flags, buffer.size(), buffer.capacity());
  buffer.serialize((uint32_t)m_flags);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    const Pattern *entry = m_ledEntries[pos];
    if (!entry) {
      continue;
    }
    // just serialize the pattern then colorset
    entry->serialize(buffer);
    // if either of these flags are present only serialize the first pattern
    if (hasFlags(MODE_FLAG_MULTI_LED | MODE_FLAG_ALL_SAME_SINGLE)) {
      break;
    }
  }
}

void Mode::unserialize(SerialBuffer &buffer)
{
  clearPatterns();
  buffer.unserialize((uint32_t *)&m_flags);
  DEBUGF("Loaded mode flags: %x", m_flags);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos > LED_FIRST && hasFlags(MODE_FLAG_ALL_SAME_SINGLE)) {
      m_ledEntries[pos] = PatternBuilder::make(m_ledEntries[0]->getPatternID());
    } else {
      m_ledEntries[pos] = PatternBuilder::unserialize(buffer);
    }
    if (!m_ledEntries[pos]) {
      // error
      return;
    }
    if (pos > LED_FIRST && hasFlags(MODE_FLAG_ALL_SAME_SINGLE)) {
      // need to bind the colorset and the position
      m_ledEntries[pos]->bind(m_ledEntries[0]->getColorset(), pos);
    } else {
      // the led positions aren't stored, need to restore them manually
      m_ledEntries[pos]->setLedPos(pos);
    }
    // if either of these flags are present only unserialize one pattern
    if (hasFlags(MODE_FLAG_MULTI_LED)) {
      // done
      break;
    }
  }
}

bool Mode::bind(PatternID id, const Colorset *set)
{
  if (isMultiLedPatternID(id)) {
    return bindMulti(id, set);
  }
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (!bindSingle(id, set, pos)) {
      return false;
    }
  }
  addFlags(MODE_FLAG_ALL_SAME_SINGLE);
  return true;
}

// bind a single led pattern and colorset to individual LED
bool Mode::bindSingle(PatternID id, const Colorset *set, LedPos pos)
{
  // don't allow binding single on invalid LED, or a mode that already has multi
  if (pos > LED_LAST || hasFlags(MODE_FLAG_MULTI_LED)) {
    return false;
  }
  SingleLedPattern *pat = PatternBuilder::makeSingle(id);
  if (!pat) {
    return false;
  }
  pat->bind(set, pos);
  delete m_ledEntries[pos];
  m_ledEntries[pos] = pat;
  return true;
}

// bind a multi led pattern and colorset to all LEDs
bool Mode::bindMulti(PatternID id, const Colorset *set)
{
  MultiLedPattern *pat = PatternBuilder::makeMulti(id);
  if (!pat) {
    return false;
  }
  pat->bind(set);
  unbindAll();
  m_multiPat = pat;
  addFlags(MODE_FLAG_MULTI_LED);
  return true;
}

void Mode::unbindSingle(LedPos pos)
{
  if (pos > LED_LAST || hasFlags(MODE_FLAG_MULTI_LED)) {
    return;
  }
  if (m_ledEntries[pos]) {
    delete m_ledEntries[pos];
    m_ledEntries[pos] = nullptr;
  }
  // cannot possibly be using all of the same pattern
  clearFlags(MODE_FLAG_ALL_SAME_SINGLE);
}

void Mode::unbindMulti()
{
  if (m_multiPat) {
    delete m_multiPat;
    m_multiPat = nullptr;
  }
  // no longer a multi-led mode
  clearFlags(MODE_FLAG_MULTI_LED);
}

void Mode::unbindAll()
{
  unbindMulti();
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    unbindSingle(pos);
  }
}

const Pattern *Mode::getPattern(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_ledEntries[pos];
}

const Colorset *Mode::getColorset(LedPos pos) const
{
  if (pos > LED_LAST || !m_ledEntries[pos]) {
    // try to return 0 if possible
    if (m_ledEntries[0]) {
      return m_ledEntries[0]->getColorset();
    }
    return nullptr;
  }
  return m_ledEntries[pos]->getColorset();
}

PatternID Mode::getPatternID(LedPos pos) const
{
  if (pos > LED_LAST || !getPattern()) {
    return PATTERN_FIRST;
  }
  return getPattern()->getPatternID();
}

bool Mode::setPattern(PatternID pat, LedPos pos)
{
  // if it's a multi pattern ID then just set the multi pattern slot
  if (isMultiLedPatternID(pat)) {
    return setMultiPat(pat);
  }
  // if a specific LED was provided
  if (pos < LED_COUNT) {
    clearFlags(MODE_FLAG_ALL_SAME_SINGLE);
    return setSinglePat(pat, pos);
  }
  // otherwise iterate all of the LEDs and set single led patterns
  for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
    if (!setSinglePat(pat, p)) {
      // error
    }
  }
  return true;
}

bool Mode::setSinglePat(PatternID pat, LedPos pos)
{
  SingleLedPattern *newPat = PatternBuilder::makeSingle(pat);
  if (!newPat) {
    // failed to build new pattern, user gave multiled pattern id?
    return false;
  }
  newPat->bind(getColorset(pos), pos);
  clearPattern(pos);
  m_ledEntries[pos] = newPat;
  return true;
}

bool Mode::setMultiPat(PatternID pat)
{
  MultiLedPattern *newPat = PatternBuilder::makeMulti(pat);
  if (!newPat) {
    return false;
  }
  // initialize the new pattern with the old colorset
  // if there isn't already a pattern
  newPat->bind(m_multiPat ? m_multiPat->getColorset() : nullptr);
  // clear any stored patterns
  clearPatterns();
  // update the multi pattern
  m_multiPat = newPat;
  return true;
}

// this will in-place change the colorset for a given led or all slots
bool Mode::setColorset(const Colorset *set, LedPos pos)
{
  if (hasFlags(MODE_FLAG_MULTI_LED)) {
    pos = LED_FIRST;
  }
  if (pos < LED_COUNT) {
    if (!m_ledEntries[pos]) {
      return false;
    }
    m_ledEntries[pos]->setColorset(set);
    clearFlags(MODE_FLAG_ALL_SAME_SINGLE);
    return true;
  }
  // otherwise set all of the colorsets
  for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
    if (!m_ledEntries[p]) {
      continue;
    }
    m_ledEntries[p]->setColorset(set);
  }
  return true;
}

void Mode::clearPatterns()
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    clearPattern(pos);
  }
}

void Mode::clearPattern(LedPos pos)
{
  if (!m_ledEntries[pos]) {
    return;
  }
  delete m_ledEntries[pos];
  m_ledEntries[pos] = nullptr;
}

void Mode::clearColorsets()
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (!m_ledEntries[pos]) {
      continue;
    }
    m_ledEntries[pos]->clearColorset();
  }
}
