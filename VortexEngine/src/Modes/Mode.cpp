#include "Mode.h"

#include "../VortexEngine.h"

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

bool Mode::saveToBuffer(ByteStream &modeBuffer) const
{
  // serialize the engine version into the modes buffer
  VortexEngine::serializeVersion(modeBuffer);
  // serialize the total number of leds and global brightness
  modeBuffer.serialize((uint8_t)LED_COUNT);
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
  uint8_t ledCount = 0;
  // unserialize the number of leds
  modeBuffer.unserialize(&ledCount);
  // now just unserialize the list of modes
  if (!unserialize(modeBuffer, ledCount)) {
    return false;
  }
  // then initialize the mode so that it is ready to play
  init();
  return true;
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

bool Mode::unserialize(ByteStream &buffer, uint32_t numLeds)
{
  clearPatterns();
  // unserialize the flags value
  uint32_t flags = 0;
  buffer.unserialize(&flags);
  // unserialize the first pattern
  m_ledEntries[0] = PatternBuilder::unserialize(buffer);
  // if there is no first pattern, or the flags indicate it's a multi-led
  // pattern then there's nothing more to unserialize and we're done
  if (!m_ledEntries[0] || (flags & MODE_FLAG_MULTI_LED)) {
    return true;
  }
  // if it's an 'all same single' pattern where it's the same single led pattern
  // repeated across all leds then we can just re-apply the first led that was
  // just unserialized to each of the other leds
  if (flags & MODE_FLAG_ALL_SAME_SINGLE) {
    Pattern *firstPat = m_ledEntries[0];
    PatternID firstID = firstPat->getPatternID();
    const Colorset *firstSet = firstPat->getColorset();
    PatternArgs firstArgs;
    firstPat->getArgs(firstArgs);
    for (LedPos pos = (LedPos)(LED_FIRST + 1); pos < LED_COUNT; ++pos) {
      if (!setSinglePat(pos, firstID, &firstArgs, firstSet)) {
        // fail?
        continue;
      }
    }
    return true;
  }
  // we already loaded led 0 so the led position starts at 1
  LedPos pos = (LedPos)(LED_FIRST + 1);
  // unserialize the rest of the leds out of the savefile which 
  // is numleds - 1 because we already loaded the first led
  for (uint32_t i = 0; i < (numLeds - 1); ++i) {
    Pattern *pat = PatternBuilder::unserialize(buffer);
    // if we have loaded all of our available leds
    if (pos >= LED_COUNT) {
      // then just discard this pattern we cannot apply it
      delete pat;
      continue;
    }
    if (!pat) {
      ERROR_LOG("Failed to unserialize pattern from buffer");
      return false;
    }
    // must bind the pattern to position so the pattern knows which led
    pat->bind(pos);
    // then store the pattern in the leds array at the right position
    m_ledEntries[pos] = pat;
    // move forward to next position
    pos++;
  }
  // at this point if our pos isn't our LED_LAST then that means the
  // savefile had less entries in it than we can support and we need
  // to repeat those entries to fill up our slots
  if (pos < LED_COUNT) {
    LedPos src = LED_FIRST;
    for(;pos < LED_COUNT; ++pos) {
      Pattern *pat = m_ledEntries[src];
      if (!pat) {
        continue;
      }
      PatternArgs args;
      pat->getArgs(args);
      setSinglePat(pos, pat->getPatternID(), &args, pat->getColorset());
      // increment the src led but wrap at the ledcount so for example
      // a savefile with only 3 leds saved will come out as ABCABCABCA
      src = (LedPos)((src + 1) % numLeds);
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
