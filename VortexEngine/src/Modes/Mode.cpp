#include "Mode.h"

#include "../VortexEngine.h"

#include "../Patterns/single/SingleLedPattern.h"
#include "../Patterns/multi/MultiLedPattern.h"
#include "../Patterns/PatternBuilder.h"
#include "../Patterns/Pattern.h"
#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Colors/Colorset.h"
#include "../Memory/Memory.h"
#include "../Log/Log.h"

#if FIXED_LED_COUNT == 0
// for internal reference to the led count
#define MODE_LEDCOUNT m_numLeds
Mode::Mode(uint32_t numLeds) :
#if VORTEX_SLIM == 0
  m_multiPat(nullptr),
#endif
  m_numLeds(numLeds),
  m_singlePats(nullptr)
{
  setLedCount(m_numLeds);
}
Mode::Mode() :
  Mode(LED_COUNT)
{
}
#else
// for internal reference to the led count
#define MODE_LEDCOUNT LED_COUNT
Mode::Mode() :
#if VORTEX_SLIM == 0
  m_multiPat(nullptr),
#endif
  m_singlePats()
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    m_singlePats[i] = nullptr;
  }
}
#endif

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

Mode::Mode(PatternID id, const PatternArgs *args, const Colorset *set) :
  Mode()
{
  setPattern(id, args, set);
}

Mode::Mode(const Mode *other) :
  Mode()
{
  if (!other) {
    return;
  }
  *this = *other;
}

Mode::~Mode()
{
  clearPatterns();
#if FIXED_LED_COUNT == 0
  free(m_singlePats);
#endif
}

// copy and assignment operators
Mode::Mode(const Mode &other) :
  Mode()
{
  *this = other;
}

void Mode::operator=(const Mode &other)
{
#if FIXED_LED_COUNT == 0
  setLedCount(other.getLedCount());
#endif
  clearPatterns();
#if VORTEX_SLIM == 0
  if (other.m_multiPat) {
    m_multiPat = PatternBuilder::dupe(other.m_multiPat);
  }
#endif
  for (uint32_t i = 0; i < other.getLedCount(); ++i) {
    Pattern *otherPat = other.m_singlePats[i];
    if (!otherPat) {
      continue;
    }
    // todo: dupeSingle?
    m_singlePats[i] = PatternBuilder::dupe(otherPat);
  }
}

// equality operators
bool Mode::operator==(const Mode &other) const
{
  return equals(&other);
}

bool Mode::operator!=(const Mode &other) const
{
  return !equals(&other);
}

void Mode::init()
{
#if VORTEX_SLIM == 0
  // initialize the multi pattern if it's present
  if (m_multiPat) {
    m_multiPat->init();
  }
#endif
  // otherwise regular init
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    // grab the entry for this led and initialize it
    Pattern *entry = m_singlePats[pos];
    if (!entry) {
      continue;
    }
    entry->init();
  }
}

void Mode::play()
{
#if VORTEX_SLIM == 0
  // play multi pattern first so that the singles can override
  if (m_multiPat) {
    m_multiPat->play();
  }
#endif
  // now iterate all singles and play
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    // grab the entry for this led
    Pattern *entry = m_singlePats[pos];
    if (!entry) {
      // incomplete pattern/set or empty slot
      continue;
    }
    // play the current pattern with current color set on the current finger
    entry->play();
  }
}

bool Mode::saveToBuffer(ByteStream &modeBuffer) const
{
  // serialize the engine version into the modes buffer
  VortexEngine::serializeVersion(modeBuffer);
  // serialize all mode data into the modeBuffer
  serialize(modeBuffer);
  DEBUG_LOGF("Serialized mode, uncompressed size: %u", modeBuffer.size());
  return modeBuffer.compress();
}

bool Mode::loadFromBuffer(ByteStream &modeBuffer)
{
  if (!modeBuffer.decompress()) {
    // failed to decompress the mode
    return false;
  }
  // reset the unserializer index before unserializing anything
  modeBuffer.resetUnserializer();
  uint8_t major = 0;
  uint8_t minor = 0;
  // unserialize the vortex version
  modeBuffer.unserialize(&major);
  modeBuffer.unserialize(&minor);
  // check the version for incompatibility
  if (!VortexEngine::checkVersion(major, minor)) {
    // incompatible version
    ERROR_LOGF("Incompatible savefile version: %u.%u", major, minor);
    return false;
  }
  // now just unserialize the list of patterns
  if (!unserialize(modeBuffer)) {
    return false;
  }
  // then initialize the mode so that it is ready to play
  init();
  return true;
}

void Mode::serialize(ByteStream &buffer) const
{
  // serialize the number of leds
  buffer.serialize((uint8_t)MODE_LEDCOUNT);
#if FIXED_LED_COUNT == 0
  // empty mode?
  if (!MODE_LEDCOUNT) {
    return;
  }
#endif
  // serialize the flags
  uint32_t flags = getFlags();
  buffer.serialize(flags);
#if VORTEX_SLIM == 0
  // serialiaze the multi led?
  if ((flags & MODE_FLAG_MULTI_LED) && m_multiPat) {
    // serialize the multi led
    m_multiPat->serialize(buffer);
  }
#endif
  // if no single leds then just stop here
  if (!(flags & MODE_FLAG_SINGLE_LED)) {
    return;
  }
  // if there are any sparse singles (spaces) then we need to
  // serialize an led map of which singles are set
  if (flags & MODE_FLAG_SPARSE_SINGLES) {
    buffer.serialize((uint32_t)getSingleLedMap());
  }
  // then iterate each single led and serialize it
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    const Pattern *entry = m_singlePats[pos];
    if (!entry) {
      continue;
    }
    // just serialize the pattern then colorset
    entry->serialize(buffer);
    // if they are all same single then only serialize one
    if (flags & MODE_FLAG_ALL_SAME_SINGLE) {
      break;
    }
  }
}

// this is a hairy function, but a bit of a necessary complexity
bool Mode::unserialize(ByteStream &buffer)
{
  clearPatterns();
  uint8_t ledCount = 0;
  // unserialize the number of leds
  buffer.unserialize(&ledCount);
#if FIXED_LED_COUNT == 0
  // it's important that we only increase the led count if necessary
  // otherwise we may end up reducing our led count and only rendering
  // a few leds on the device
  if (ledCount > m_numLeds) {
    // adjust the internal LED count of the mode itself, this allows it to
    // actually manage that many patterns at once
    setLedCount(ledCount);
  }
#endif
  if (!ledCount) {
    // empty mode?
    return true;
  }
  // unserialize the flags value
  uint32_t flags = 0;
  buffer.unserialize(&flags);
  // if there is a multi led pattern then unserialize it
  if (flags & MODE_FLAG_MULTI_LED) {
#if VORTEX_SLIM == 1
    // unserialize the multi pattern but just discard it
    PatternBuilder::unserialize(buffer);
#else
    // otherwise in normal build actually unserialize it
    m_multiPat = PatternBuilder::unserialize(buffer);
    m_multiPat->init();
#endif
  }
  // if there is no single led patterns just stop here
  if (!(flags & MODE_FLAG_SINGLE_LED)) {
    return true;
  }
  // is there an led map to unserialize? if not default to all
  LedMap map = (1 << ledCount) - 1;
  if (flags & MODE_FLAG_SPARSE_SINGLES) {
    buffer.unserialize((uint32_t *)&map);
  }
  // unserialize all singleled patterns into their positions
  Pattern *firstPat = nullptr;
  MAP_FOREACH_LED(map) {
    if (pos >= LED_COUNT) {
      // in case the map encodes led positions this device doesn't support
      break;
    }
    if (!firstPat) {
      // save the first pattern so that it can be duped if this is 'all same'
      m_singlePats[pos] = firstPat = PatternBuilder::unserialize(buffer);
    } else if (flags & MODE_FLAG_ALL_SAME_SINGLE) {
      // if all same then just dupe first
      m_singlePats[pos] = PatternBuilder::dupe(firstPat);
    } else {
      // otherwise unserialize the pattern like normal
      m_singlePats[pos] = PatternBuilder::unserialize(buffer);
    }
    m_singlePats[pos]->bind(pos);
  }
  // there is a few different possibilities here:
  //    1. The provided ledCount is less than our current LED_COUNT
  //      -> if this happens we need to repeat the first ledCount leds
  //         into the remaining LED_COUNT - ledCount
  //    2. The provided ledCount is more than our current LED_COUNT
  //      -> if this happens then we can just chop the leds and continue
  //    3. The provided ledCount is the same as our current LED_COUNT
  //      -> if this happens all is good we can continue
  if (ledCount >= LED_COUNT) {
    // in this case we either chopped some off or have the exact amount
    return true;
  }
  // in this case we have to repeat them so we loop LED_COUNT - ledCount
  // times and walk the first ledCount that are already set and copy them
  LedPos src = LED_FIRST;
  // start from ledCount (the first index we didn't load) and loop till
  // LED_COUNT and dupe the pattern in the src position, but wrap the src
  // around at ledCount so that we repeat the first ledCount over again
  for (LedPos pos = (LedPos)ledCount; pos < LED_COUNT; ++pos) {
    m_singlePats[pos] = PatternBuilder::dupe(m_singlePats[src]);
    m_singlePats[pos]->bind(pos);
    src = (LedPos)((src + 1) % ledCount);
  }
  return true;
}

#if FIXED_LED_COUNT == 0
// change the internal pattern count in the mode object
void Mode::setLedCount(uint8_t numLeds)
{
  if (m_singlePats) {
    clearPatterns();
    free(m_singlePats);
  }
  m_numLeds = numLeds;
  m_singlePats = (Pattern **)vcalloc(m_numLeds, sizeof(Pattern *));
  if (!m_singlePats) {
    // big error
    return;
  }
}
#endif

uint8_t Mode::getLedCount() const
{
#if FIXED_LED_COUNT == 0
  return m_numLeds;
#else
  return LED_COUNT;
#endif
}

const Pattern *Mode::getPattern() const
{
  return ((Mode *)this)->getPattern();
}

Pattern *Mode::getPattern()
{
#if VORTEX_SLIM == 0
  if (m_multiPat) {
    return m_multiPat;
  }
#endif
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (m_singlePats[pos]) {
      return m_singlePats[pos];
    }
  }
  return nullptr;
}

const Pattern *Mode::getPatternAt(LedPos pos) const
{
  return ((Mode *)this)->getPatternAt(pos);
}

Pattern *Mode::getPatternAt(LedPos pos)
{
  // if fetching for 'all' leds just return the first pattern
  if (pos >= MODE_LEDCOUNT) {
    pos = LED_FIRST;
  }
  return m_singlePats[pos];
}

const Pattern *Mode::getMultiPat() const
{
  return ((Mode *)this)->getMultiPat();
}

Pattern *Mode::getMultiPat()
{
#if VORTEX_SLIM == 1
  return nullptr;
#else
  return m_multiPat;
#endif
}

const Colorset *Mode::getColorset() const
{
  return ((Mode *)this)->getColorset();
}

Colorset *Mode::getColorset()
{
#if VORTEX_SLIM == 0
  if (m_multiPat) {
    return m_multiPat->getColorset();
  }
#endif
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (m_singlePats[pos]) {
      return m_singlePats[pos]->getColorset();
    }
  }
  return nullptr;
}

const Colorset *Mode::getColorsetAt(LedPos pos) const
{
  return ((Mode *)this)->getColorsetAt(pos);
}

Colorset *Mode::getColorsetAt(LedPos pos)
{
  // if fetching for 'all' leds just return the first
  if (pos >= MODE_LEDCOUNT) {
    pos = LED_FIRST;
  }
  if (!m_singlePats[pos]) {
    return nullptr;
  }
  return m_singlePats[pos]->getColorset();
}

const Colorset *Mode::getMultiColorset() const
{
  return ((Mode *)this)->getMultiColorset();
}

Colorset *Mode::getMultiColorset()
{
#if VORTEX_SLIM == 0
  if (!m_multiPat) {
    return nullptr;
  }
  return m_multiPat->getColorset();
#else
  return nullptr;
#endif
}

PatternID Mode::getPatternID() const
{
#if VORTEX_SLIM == 0
  if (m_multiPat) {
    return getMultiPatID();
  }
#endif
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (m_singlePats[pos]) {
      return m_singlePats[pos]->getPatternID();
    }
  }
  return PATTERN_NONE;
}

PatternID Mode::getPatternIDAt(LedPos pos) const
{
  // if fetching for 'all' leds just return the first
  if (pos >= MODE_LEDCOUNT) {
    pos = LED_FIRST;
  }
  if (!m_singlePats[pos]) {
    return PATTERN_NONE;
  }
  return m_singlePats[pos]->getPatternID();
}

PatternID Mode::getMultiPatID() const
{
#if VORTEX_SLIM == 0
  if (!m_multiPat) {
    return PATTERN_NONE;
  }
  return m_multiPat->getPatternID();
#else
  return PATTERN_NONE;
#endif
}

bool Mode::equals(const Mode *other) const
{
  if (!other) {
    return false;
  }
  // compare the led count
  if (other->getLedCount() != MODE_LEDCOUNT) {
    return false;
  }
#if VORTEX_SLIM == 0
  // compare the multi pattern
  if (m_multiPat && !m_multiPat->equals(other->m_multiPat)) {
    return false;
  }
#endif
  // compare all the singles
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    // if entry is valid, do a comparison
    if (m_singlePats[pos]) {
      // checks if other is not null and equal
      if (!m_singlePats[pos]->equals(other->m_singlePats[pos])) {
        return false;
      }
      continue;
    }
    // current is null, check if other is valid
    if (other->m_singlePats[pos]) {
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
  for (LedPos p = LED_FIRST; p < MODE_LEDCOUNT; ++p) {
    if (!setPatternAt(p, pat, args, set)) {
      ERROR_LOGF("Failed to set single pattern %u", p);
      return false;
    }
  }
  return true;
}

bool Mode::setPatternAt(LedPos pos, PatternID pat, const PatternArgs *args, const Colorset *set)
{
  if (pos >= MODE_LEDCOUNT) {
    return setPattern(pat, args, set);
  }
  SingleLedPattern *newPat = PatternBuilder::makeSingle(pat, args);
  if (!newPat) {
    // failed to build new pattern, user gave multiled pattern id?
    return false;
  }
  return setPatternAt(pos, newPat, set);
}

bool Mode::setPatternAt(LedPos pos, SingleLedPattern *pat, const Colorset *set)
{
  if (!pat) {
    return false;
  }
  if (pos >= MODE_LEDCOUNT) {
    PatternArgs args;
    pat->getArgs(args);
    return setPattern(pat->getPatternID(), &args, set);
  }
  // bind the position and colorset, if the colorset is missing then just
  // bind the previously assigned colorset, if that is also missing then
  // try to grab the colorset from the first led. This could happen for
  // example if a multi led pattern was set and we're not setting single
  // led patterns on all the fingers
  if (!set) {
    set = getColorsetAt(pos);
    if (!set) {
      set = getColorset();
    }
  }
  pat->bind(pos);
  pat->setColorset(set);
  clearPatternAt(pos);
  m_singlePats[pos] = pat;
  return true;
}

bool Mode::setPatternAt(LedMap map, PatternID pat, const PatternArgs *args, const Colorset *set)
{
  MAP_FOREACH_LED(map) {
    if (!setPatternAt(pos, pat, args, set)) {
      return false;
    }
  }
  return true;
}

bool Mode::setPatternAt(LedMap map, SingleLedPattern *pat, const Colorset *set)
{
  MAP_FOREACH_LED(map) {
    if (!setPatternAt(pos, pat, set)) {
      return false;
    }
  }
  return true;
}

bool Mode::setMultiPat(PatternID pat, const PatternArgs *args, const Colorset *set)
{
#if VORTEX_SLIM == 0
  MultiLedPattern *newPat = PatternBuilder::makeMulti(pat, args);
  if (!newPat) {
    return false;
  }
  return setMultiPat(newPat, set);
#else
  return false;
#endif
}

bool Mode::setMultiPat(MultiLedPattern *pat, const Colorset *set)
{
#if VORTEX_SLIM == 0
  if (!pat) {
    return false;
  }
  // initialize the new pattern with the old colorset
  // if there isn't already a pattern
  pat->setColorset(set ? set : getColorset());
  // clear any stored patterns
  clearMultiPat();
  // update the multi pattern
  m_multiPat = pat;
  return true;
#else
  return false;
#endif
}

bool Mode::setColorset(const Colorset *set)
{
#if VORTEX_SLIM == 0
  if (isMultiLed()) {
    m_multiPat->setColorset(set);
  }
#endif
  // otherwise set all of the colorsets
  for (LedPos p = LED_FIRST; p < MODE_LEDCOUNT; ++p) {
    if (!m_singlePats[p]) {
      continue;
    }
    m_singlePats[p]->setColorset(set);
  }
  return true;
}

// set colorset at a specific position
bool Mode::setColorsetAt(LedPos pos, const Colorset *set)
{
  if (pos >= MODE_LEDCOUNT) {
    return setColorset(set);
  }
  if (!m_singlePats[pos]) {
    return false;
  }
  m_singlePats[pos]->setColorset(set);
  return true;
}

// set colorset at each position in a map
bool Mode::setColorsetAt(LedMap map, const Colorset *set)
{
  MAP_FOREACH_LED(map) {
    if (!setColorsetAt(pos, set)) {
      return false;
    }
  }
  return true;
}

bool Mode::setMultiColorset(const Colorset *set)
{
#if VORTEX_SLIM == 0
  if (!m_multiPat) {
    return false;
  }
  m_multiPat->setColorset(set);
  return true;
#else
  return false;
#endif
}

void Mode::clearPatterns()
{
#if FIXED_LED_COUNT == 0
  if (!m_singlePats) {
    return;
  }
#endif
  clearMultiPat();
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    clearPatternAt(pos);
  }
}

void Mode::clearPatternAt(LedPos pos)
{
#if FIXED_LED_COUNT == 0
  if (!m_singlePats) {
    return;
  }
#endif
  if (!m_singlePats[pos]) {
    return;
  }
  delete m_singlePats[pos];
  m_singlePats[pos] = nullptr;
}

void Mode::clearPatternAt(LedMap map)
{
  MAP_FOREACH_LED(map) {
    clearPatternAt(pos);
  }
}

void Mode::clearMultiPat()
{
#if VORTEX_SLIM == 0
  if (!m_multiPat) {
    return;
  }
  delete m_multiPat;
  m_multiPat = nullptr;
#endif
}

void Mode::clearColorsets()
{
  clearMultiColorset();
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    clearColorsetAt(pos);
  }
}

void Mode::clearColorsetAt(LedPos pos)
{
#if FIXED_LED_COUNT == 0
  if (!m_singlePats) {
    return;
  }
#endif
  if (!m_singlePats[pos]) {
    return;
  }
  m_singlePats[pos]->clearColorset();
}

void Mode::clearColorsetAt(LedMap map)
{
  MAP_FOREACH_LED(map) {
    clearColorsetAt(pos);
  }
}

void Mode::clearMultiColorset()
{
#if VORTEX_SLIM == 0
  if (!m_multiPat) {
    return;
  }
  m_multiPat->clearColorset();
#endif
}

uint32_t Mode::getFlags() const
{
  uint32_t flags = 0;
  if (hasMultiLed()) flags |= MODE_FLAG_MULTI_LED;
  if (hasSingleLed()) flags |= MODE_FLAG_SINGLE_LED;
  if (hasSameSingleLed()) flags |= MODE_FLAG_ALL_SAME_SINGLE;
  return flags;
}

// whether a multi-led pattern is present in the mode
bool Mode::hasMultiLed() const
{
#if VORTEX_SLIM == 0
  return (m_multiPat != nullptr);
#else
  return false;
#endif
}

// whether at least one single-led pattern is present in the mode
bool Mode::hasSingleLed() const
{
  for (uint32_t i = LED_FIRST; i < MODE_LEDCOUNT; ++i) {
    if (m_singlePats[i]) {
      return true;
    }
  }
  return false;
}

// are all the single led patterns and colorsets equal?
bool Mode::hasSameSingleLed() const
{
  Pattern *firstPat = nullptr;
  for (uint32_t i = LED_FIRST + 1; i < MODE_LEDCOUNT; ++i) {
    if (!m_singlePats[i]) {
      continue;
    }
    if (!firstPat) {
      firstPat = m_singlePats[i];
      continue;
    }
    if (!firstPat->equals(m_singlePats[i])) {
      return false;
    }
  }
  // all the same if at least one pattern was found
  return (firstPat != nullptr);
}

bool Mode::hasSparseSingleLed() const
{
  switch (getSingleLedMap()) {
  case MAP_LED_ALL:
  case 0:
    // if all or none are set it's not sparse
    return false;
  default:
    // if anything else is set it's sparse
    return true;
  }
}

LedMap Mode::getSingleLedMap() const
{
  LedMap map = 0;
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; pos++) {
    if (m_singlePats[pos]) {
      setLed(map, pos);
    }
  }
  return map;
}

// is this a multi-led pattern in the mode?
bool Mode::isMultiLed() const
{
#if VORTEX_SLIM == 0
  return hasMultiLed() && !hasSingleLed();
#else
  return false;
#endif
}

#if MODES_TEST == 1
#include <assert.h>

// Mode::Test function
void Mode::test()
{
  // Test setPattern
  Mode modeTest;
  PatternID testPatternID = PATTERN_SOLID;
  PatternArgs testPatternArgs = { 100, 255, 0, 10 };
  Colorset testColorset(0, 0, 255);

  modeTest.setPattern(testPatternID, &testPatternArgs, &testColorset);

  // Check if the pattern and colorset were set correctly
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeTest.getPatternID(pos) == testPatternID);
    assert(modeTest.getColorset(pos)->equals(&testColorset));
  }
  DEBUG_LOG("Test setPattern passed.");

  // Test setColorsetAt
  Colorset newColorset(255, 255, 0);
  LedPos setColorsetPosition = LED_3;
  modeTest.setColorsetAt(&newColorset, setColorsetPosition);

  // Check if the colorset was set correctly at the specified position
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == setColorsetPosition) {
      assert(modeTest.getColorset(pos)->equals(&newColorset));
    } else {
      assert(modeTest.getColorset(pos)->equals(&testColorset));
    }
  }
  DEBUG_LOG("Test setColorsetAt passed.");

  // Add more test cases as needed

  // Test clearPattern
  Mode modeClearTest;
  modeClearTest.setPattern(testPatternID, &testPatternArgs, &testColorset);
  modeClearTest.clearPattern();

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeClearTest.getPatternID(pos) == PATTERN_NONE);
  }
  DEBUG_LOG("Test clearPattern passed.");

  // Test setPatternAt
  Mode modeSetPatternAtTest;
  LedPos setPatternAtPosition = LED_5;
  modeSetPatternAtTest.setPatternAt(setPatternAtPosition, testPatternID, &testPatternArgs, &testColorset);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == setPatternAtPosition) {
      assert(modeSetPatternAtTest.getPatternID(pos) == testPatternID);
    } else {
      assert(modeSetPatternAtTest.getPatternID(pos) == PATTERN_NONE);
    }
  }
  DEBUG_LOG("Test setPatternAt passed.");

  // Test clearPatternAt
  Mode modeClearPatternAtTest;
  LedPos clearPatternAtPosition = LED_7;
  modeClearPatternAtTest.setPattern(testPatternID, &testPatternArgs, &testColorset);
  modeClearPatternAtTest.clearPatternAt(clearPatternAtPosition);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == clearPatternAtPosition) {
      assert(modeClearPatternAtTest.getPatternID(pos) == PATTERN_NONE);
    } else {
      assert(modeClearPatternAtTest.getPatternID(pos) == testPatternID);
    }
  }
  DEBUG_LOG("Test clearPatternAt passed.");

  // Test setPatternAt for a specific position
  Mode modePatternAtTest;
  Colorset colorset(0xFF0000, 0x00FF00, 0x0000FF);
  PatternID patternId = PATTERN_SOLID;
  assert(modePatternAtTest.setPatternAt(LED_FIRST, patternId, nullptr, &colorset));
  assert(modePatternAtTest.getPatternID(LED_FIRST) == patternId);
  assert(modePatternAtTest.getColorset(LED_FIRST)->equals(&colorset));
  DEBUG_LOG("Test clearPatternAt passed.");

  // Test setColorsetAt for a specific position
  Mode modeSetColorsetAtTest;
  assert(modeSetColorsetAtTest.setPattern(PATTERN_BASIC));
  assert(modeSetColorsetAtTest.setColorsetAt(&colorset, LED_FIRST));
  assert(modeSetColorsetAtTest.getColorset(LED_FIRST));
  assert(modeSetColorsetAtTest.getColorset(LED_FIRST)->equals(&colorset));
  DEBUG_LOG("Test setColorsetAt passed.");

  // Test setMultiPat
  Mode modeSetMultiPatTest;
  assert(modeSetMultiPatTest.setMultiPat(PATTERN_MULTI_FIRST, nullptr, &colorset));
  assert(modeSetMultiPatTest.isMultiLed());
  assert(modeSetMultiPatTest.getPatternID() == PATTERN_MULTI_FIRST);
  assert(modeSetMultiPatTest.getColorset()->equals(&colorset));
  DEBUG_LOG("Test setMultiPat passed.");

  // Test clearColorset
  Mode modeClearColorsetTest;
  assert(modeClearColorsetTest.setPatternAt(LED_FIRST, patternId, nullptr, &colorset));
  modeClearColorsetTest.clearColorset();
  assert(modeClearColorsetTest.getColorset(LED_FIRST)->numColors() == 0);
  DEBUG_LOG("Test clearColorset passed.");

  // Test clearPatternAt for a specific position
  modeClearPatternAtTest.init();
  assert(modeClearPatternAtTest.setPatternAt(LED_FIRST, patternId, nullptr, &colorset));
  modeClearPatternAtTest.clearPatternAt(LED_FIRST);
  assert(modeClearPatternAtTest.getPattern(LED_FIRST) == nullptr);
  assert(modeClearPatternAtTest.getColorset(LED_FIRST) == nullptr);
  DEBUG_LOG("Test clearColorsetAt passed.");

  // Test copy constructor
  Mode modeCopyConstructorTest;
  assert(modeCopyConstructorTest.setPatternAt(LED_FIRST, patternId, nullptr, &colorset));
  Mode modeCopyConstructorTestCopy(modeCopyConstructorTest);
  assert(modeCopyConstructorTestCopy.getPatternID(LED_FIRST) == patternId);
  assert(modeCopyConstructorTestCopy.getColorset(LED_FIRST)->equals(&colorset));
  DEBUG_LOG("Test copy constructor passed.");

  // Test assignment operator
  Mode modeAssignmentOperatorTest;
  assert(modeAssignmentOperatorTest.setPatternAt(LED_FIRST, patternId, nullptr, &colorset));
  Mode modeAssignmentOperatorTestCopy;
  modeAssignmentOperatorTestCopy = modeAssignmentOperatorTest;
  assert(modeAssignmentOperatorTestCopy.getPatternID(LED_FIRST) == patternId);
  assert(modeAssignmentOperatorTestCopy.getColorset(LED_FIRST)->equals(&colorset));
  DEBUG_LOG("Test assignment operator passed.");

  // Test equality operator
  Mode modeEqualityOperatorTest1;
  Mode modeEqualityOperatorTest2;
  assert(modeEqualityOperatorTest1.setPatternAt(LED_FIRST, patternId, nullptr, &colorset));
  assert(modeEqualityOperatorTest2.setPatternAt(LED_FIRST, patternId, nullptr, &colorset));
  assert(modeEqualityOperatorTest1 == modeEqualityOperatorTest2);
  DEBUG_LOG("Test equality operator passed.");

  // Test inequality operator
  Mode modeInequalityOperatorTest1;
  Mode modeInequalityOperatorTest2;
  assert(modeInequalityOperatorTest1.setPatternAt(LED_FIRST, patternId, nullptr, &colorset));
  assert(modeInequalityOperatorTest2.setPatternAt(LED_LAST, patternId, nullptr, &colorset));
  assert(modeInequalityOperatorTest1 != modeInequalityOperatorTest2);
  DEBUG_LOG("Test inequality operator passed.");

  // Test isSameSingleLed
  Mode modeSameSingleLedTest;
  assert(modeSameSingleLedTest.setPattern(patternId, nullptr, &colorset));
  assert(modeSameSingleLedTest.isSameSingleLed());
  DEBUG_LOG("Test isSameSingleLed passed.");

  // Test getLedCount
#if FIXED_LED_COUNT == 0
  Mode modeGetLedCountTest(5);
  assert(modeGetLedCountTest.getLedCount() == 5);
  Mode modeSetLedCountTest2(3);
  modeSetLedCountTest2.setLedCount(7);
  assert(modeSetLedCountTest2.getLedCount() == 7);
  DEBUG_LOG("Test getLedCount passed.");
#endif

}
#endif
