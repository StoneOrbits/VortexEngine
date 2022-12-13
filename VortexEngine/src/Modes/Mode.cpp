#include "Mode.h"

#include "../Patterns/single/SingleLedPattern.h"
#include "../Patterns/multi/MultiLedPattern.h"
#include "../Patterns/PatternBuilder.h"
#include "../Patterns/Pattern.h"
#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Colors/Colorset.h"
#include "../Log/Log.h"

Mode::Mode() :
  m_ledEntries()
{
}

Mode::Mode(PatternID id, const Colorset &set) :
  Mode()
{
  setPattern(id, nullptr, &set);
}

Mode::Mode(PatternID id, const PatternArgs &args, const Colorset &set) :
  Mode()
{
  setPattern(id, &args, &set);
}

Mode::~Mode()
{
  clearPatterns();
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
    // play the current pattern with current color set on the current finger
    entry->play();
    // if either of these flags are present only play the first pattern
    if (isMultiLed()) {
      //break;
    }
  }
}

void Mode::serialize(ByteStream &buffer) const
{
  uint32_t flags = getFlags();
  buffer.serialize(flags);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    const Pattern *entry = m_ledEntries[pos];
    if (!entry) {
      continue;
    }
    // just serialize the pattern then colorset
    entry->serialize(buffer);
    // if either of these flags are present only serialize the first pattern
    if (flags & (MODE_FLAG_MULTI_LED | MODE_FLAG_ALL_SAME_SINGLE)) {
      break;
    }
  }
}

bool Mode::unserialize(ByteStream &buffer)
{
  clearPatterns();
  uint32_t flags = 0;
  buffer.unserialize(&flags);
  // unserialize the first pattern
  m_ledEntries[0] = PatternBuilder::unserialize(buffer);
  if (!m_ledEntries[0] || (flags & MODE_FLAG_MULTI_LED)) {
    // done
    return true;
  }
  PatternID firstID = m_ledEntries[0]->getPatternID();
  const Colorset *firstSet = m_ledEntries[0]->getColorset();
  PatternArgs firstArgs;
  m_ledEntries[0]->getArgs(firstArgs);
  // loop from 2nd led position to last, skipping first
  for (LedPos pos = (LedPos)(LED_FIRST + 1); pos < LED_COUNT; ++pos) {
    if (flags & MODE_FLAG_ALL_SAME_SINGLE) {
      m_ledEntries[pos] = PatternBuilder::make(firstID, &firstArgs);
      if (!m_ledEntries[pos]) {
        ERROR_LOG("Failed to created pattern");
        return false;
      }
      m_ledEntries[pos]->bind(firstSet, pos);
    } else {
      m_ledEntries[pos] = PatternBuilder::unserialize(buffer);
      if (!m_ledEntries[pos]) {
        ERROR_LOG("Failed to unserialize pattern from buffer");
        return false;
      }
      // must bind to position, the position isn't serialized
      m_ledEntries[pos]->bind(pos);
    }
  }
  return true;
}

#if SAVE_TEMPLATE == 1
void Mode::saveTemplate(int level) const
{
  uint32_t flags = getFlags();
  IndentMsg(level, "\"flags\": %d,", flags);
  IndentMsg(level, "\"Leds\":[");
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    const Pattern *entry = m_ledEntries[pos];
    if (!entry) {
      continue;
    }
    IndentMsg(level + 1, "{");
    // just serialize the pattern then colorset
    entry->saveTemplate(level + 2);
    // close the Params {
    IndentMsg(level + 2, "}");
    // look screw the json standard I'm putting a comma after every entry, fix it urself.
    IndentMsg(level + 1, "},");
    // if either of these flags are present only serialize the first pattern
    if (flags & (MODE_FLAG_MULTI_LED | MODE_FLAG_ALL_SAME_SINGLE)) {
      break;
    }
  }
  IndentMsg(level, "]");
}
#endif

const Pattern *Mode::getPattern(LedPos pos) const
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_ledEntries[pos];
}

Pattern *Mode::getPattern(LedPos pos)
{
  if (pos > LED_LAST) {
    return nullptr;
  }
  return m_ledEntries[pos];
}

const Colorset *Mode::getColorset(LedPos pos) const
{
  if (pos > LED_LAST || !m_ledEntries[pos]) {
    return nullptr;
  }
  return m_ledEntries[pos]->getColorset();
}

Colorset *Mode::getColorset(LedPos pos)
{
  if (pos > LED_LAST || !m_ledEntries[pos]) {
    return nullptr;
  }
  return m_ledEntries[pos]->getColorset();
}

PatternID Mode::getPatternID(LedPos pos) const
{
  if (pos > LED_LAST || !getPattern(pos)) {
    return PATTERN_NONE;
  }
  return getPattern(pos)->getPatternID();
}

bool Mode::equals(const Mode *other) const
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // if entry is valid, do a comparison
    if (m_ledEntries[pos]) {
      // checks if other is not null and equal
      if (!m_ledEntries[pos]->equals(other->m_ledEntries[pos])) {
        return false;
      }
      continue;
    }
    // current is null, check if other is valid
    if (other->m_ledEntries[pos]) {
      return false;
    }
    // both are null
  }
  return true;
}

bool Mode::setPattern(PatternID pat, const PatternArgs *args, const Colorset *set)
{
  // if it's a multi pattern ID then just set the multi pattern slot
  if (isMultiLedPatternID(pat)) {
    return setMultiPat(pat, args, set);
  }
  // otherwise iterate all of the LEDs and set single led patterns
  for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
    if (!setSinglePat(p, pat, args, set)) {
      ERROR_LOGF("Failed to set single pattern %u", p);
      return false;
    }
  }
  return true;
}

bool Mode::setColorset(const Colorset *set)
{
  if (isMultiLed()) {
    m_multiPat->setColorset(set);
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

bool Mode::setColorsetAt(const Colorset *set, LedPos pos)
{
  if (pos >= LED_COUNT || !m_ledEntries[pos]) {
    return false;
  }
  m_ledEntries[pos]->setColorset(set);
  return true;
}

bool Mode::setSinglePat(LedPos pos, PatternID pat, const PatternArgs *args, const Colorset *set)
{
  SingleLedPattern *newPat = PatternBuilder::makeSingle(pat, args);
  if (!newPat) {
    // failed to build new pattern, user gave multiled pattern id?
    return false;
  }
  return setSinglePat(pos, newPat, set);
}

bool Mode::setSinglePat(LedPos pos, SingleLedPattern *pat, const Colorset *set)
{
  if (!pat || pos >= LED_COUNT) {
    return false;
  }
  // bind the position and colorset, if the colorset is missing then just
  // bind the previously assigned colorset, if that is also missing then
  // try to grab the colorset from the first led. This could happen for
  // example if a multi led pattern was set and we're not setting single
  // led patterns on all the fingers
  if (!set) {
    set = getColorset(pos);
    if (!set) {
      set = getColorset(LED_FIRST);
    }
  }
  pat->bind(set, pos);
  clearPattern(pos);
  m_ledEntries[pos] = pat;
  return true;
}

bool Mode::setMultiPat(PatternID pat, const PatternArgs *args, const Colorset *set)
{
  MultiLedPattern *newPat = PatternBuilder::makeMulti(pat, args);
  if (!newPat) {
    return false;
  }
  return setMultiPat(newPat, set);
}

bool Mode::setMultiPat(MultiLedPattern *pat, const Colorset *set)
{
  if (!pat) {
    return false;
  }
  // initialize the new pattern with the old colorset
  // if there isn't already a pattern
  pat->bind(set ? set : getColorset(LED_FIRST));
  // clear any stored patterns
  clearPatterns();
  // update the multi pattern
  m_multiPat = pat;
  return true;
}

uint32_t Mode::getFlags() const
{
  uint32_t flags = 0;
  if (isMultiLed()) {
    flags |= MODE_FLAG_MULTI_LED;
  } else if (isSameSingleLed()) {
    flags |= MODE_FLAG_ALL_SAME_SINGLE;
  }
  return flags;
}

// is this a multi-led pattern in the mode?
bool Mode::isMultiLed() const
{
  if (!m_ledEntries[0]) {
    return false;
  }
  return m_ledEntries[0]->hasFlags(PATTERN_FLAG_MULTI);
}

// are all the single led patterns and colorsets equal?
bool Mode::isSameSingleLed() const
{
  if (!m_ledEntries[0]) {
    return false;
  }
  if (isMultiLed()) {
    return false;
  }
  for (uint32_t i = LED_FIRST + 1; i < LED_COUNT; ++i) {
    // if any don't match 0 then no good
    if (!m_ledEntries[i] || !m_ledEntries[i]->equals(m_ledEntries[0])) {
      return false;
    }
  }
  // all the same
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
