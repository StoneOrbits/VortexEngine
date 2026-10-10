#include "Mode.h"

#include "../VortexEngine.h"

#include "../Patterns/Single/SingleLedPattern.h"
#include "../Patterns/Multi/MultiLedPattern.h"
#include "../Patterns/PatternBuilder.h"
#include "../Patterns/Pattern.h"
#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Colors/Colorset.h"
#include "../Memory/Memory.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"


#if FIXED_LED_COUNT == 0
// for internal reference to the led count
#define MODE_LEDCOUNT self->m_numLeds
#else
// for internal reference to the led count
#define MODE_LEDCOUNT LED_COUNT
#endif

void Mode_initLeds(Mode *self, uint8_t numLeds)
{
#if VORTEX_SLIM == 0
  self->m_multiPat = NULL;
#endif
#if FIXED_LED_COUNT == 0
  self->m_numLeds = numLeds;
  self->m_singlePats = (Pattern **)vmalloc(sizeof(Pattern *) * Mode_getLedCount(self));
  for (LedPos i = LED_FIRST; i < Mode_getLedCount(self); ++i) {
    self->m_singlePats[i] = NULL;
  }
  Mode_setLedCount(self, self->m_numLeds);
#else
  (void)numLeds;
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    self->m_singlePats[i] = NULL;
  }
#endif
}

void Mode_init(Mode *self)
{
#if FIXED_LED_COUNT == 0
  Mode_initLeds(self, LED_COUNT);
#else
  Mode_initLeds(self, LED_COUNT);
#endif
}

void Mode_initDefault(Mode *self, const DefaultModeEntry *entry)
{
  Mode_init(self);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    Colorset set;
    Colorset_make(&set);
    set.m_numColors = entry->leds[pos].numColors;
    for (uint8_t i = 0; i < entry->leds[pos].numColors; ++i) {
      RGBColor_setU32(&set.m_palette[i], entry->leds[pos].cols[i]);
    }
    Mode_setPattern(self, entry->leds[pos].patternID, pos, NULL, &set);
  }
}

void Mode_initPatternSet(Mode *self, PatternID id, const Colorset *set)
{
  Mode_init(self);
  Mode_setPattern(self, id, LED_ANY, NULL, set);
}

void Mode_initPatternArgsSet(Mode *self, PatternID id, const PatternArgs *args, const Colorset *set)
{
  Mode_init(self);
  Mode_setPattern(self, id, LED_ANY, args, set);
}

void Mode_copy(Mode *self, const Mode *other)
{
  Mode_init(self);
  if (!other) {
    return;
  }
  Mode_assign(self, other);
}

void Mode_destroy(Mode *self)
{
  Mode_clearPattern(self, LED_ALL);
#if FIXED_LED_COUNT == 0
  vfree(self->m_singlePats);
  self->m_singlePats = NULL;
#endif
}

void Mode_assign(Mode *self, const Mode *other)
{
  if (self == other) {
    // clearing the patterns first would destroy what is being copied
    return;
  }
#if FIXED_LED_COUNT == 0
  Mode_setLedCount(self, Mode_getLedCount(other));
#endif
  Mode_clearPattern(self, LED_ALL);
#if VORTEX_SLIM == 0
  if (other->m_multiPat) {
    self->m_multiPat = PatternBuilder_dupe(other->m_multiPat);
  }
#endif
  for (LedPos i = LED_FIRST; i < Mode_getLedCount(other); ++i) {
    Pattern *otherPat = other->m_singlePats[i];
    if (!otherPat) {
      continue;
    }
    self->m_singlePats[i] = PatternBuilder_dupe(otherPat);
  }
}


// equality operators
bool Mode_opEquals(const Mode *self, const Mode *other)
{

  return Mode_equals(self, other);
}

bool Mode_opNotEquals(const Mode *self, const Mode *other)
{

  return !Mode_equals(self, other);
}

void Mode_initPatterns(Mode *self)
{
#if VORTEX_SLIM == 0
  // initialize the multi pattern if it's present
  if (self->m_multiPat) {
    Pattern_init(self->m_multiPat);
  }
#endif
  // otherwise regular init
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    // grab the entry for this led and initialize it
    Pattern *entry = self->m_singlePats[pos];
    if (!entry) {
      continue;
    }
    Pattern_init(entry);
  }
}

void Mode_play(Mode *self)
{

#if VORTEX_SLIM == 0
  // play multi pattern first so that the singles can override
  if (self->m_multiPat) {
    Pattern_play(self->m_multiPat);
  }
#endif
  // now iterate all singles and play
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    // grab the entry for self led
    Pattern *entry = self->m_singlePats[pos];
    if (!entry) {
#if VORTEX_SLIM == 0
      // incomplete pattern/set or empty slot
      if (!self->m_multiPat) {
        // only clear if the multi pattern isn't playing
        Leds_clearIndex(pos);
      }
#else
      // just clear the index if slim, don't check for multi
      Leds_clearIndex(pos);
#endif
      continue;
    }
    // play the current pattern with current color set on the current finger
    Pattern_play(entry);
  }
}

bool Mode_saveToBuffer(const Mode *self, ByteStream *modeBuffer, uint8_t numLeds)
{

  // serialize the engine version into the modes buffer
  VortexEngine_serializeVersion(modeBuffer);
  // serialize all mode data into the modeBuffer
  Mode_serialize(self, modeBuffer, numLeds);
  DEBUG_LOGF("Serialized mode, uncompressed size: %u", ByteStream_size(modeBuffer));
  return ByteStream_compress(modeBuffer);
}

bool Mode_loadFromBuffer(Mode *self, ByteStream *modeBuffer)
{

  if (!ByteStream_decompress(modeBuffer)) {
    // failed to decompress the mode
    return false;
  }
  // reset the unserializer index before unserializing anything
  ByteStream_resetUnserializer(modeBuffer);
  uint8_t major = 0;
  uint8_t minor = 0;
  // unserialize the vortex version
  if (!ByteStream_unserialize8(modeBuffer, &major)) {
    return false;
  }
  if (!ByteStream_unserialize8(modeBuffer, &minor)) {
    return false;
  }
  // check the version for incompatibility
  if (!VortexEngine_checkVersion(major, minor)) {
    // incompatible version
    ERROR_LOGF("Incompatible savefile version: %u.%u", major, minor);
    return false;
  }
  // now just unserialize the list of patterns
  if (!Mode_unserialize(self, modeBuffer)) {
    return false;
  }
  // then initialize the mode so that it is ready to play
  Mode_initPatterns(self);
  return true;
}

bool Mode_serialize(const Mode *self, ByteStream *buffer, uint8_t numLeds)
{

  if (!numLeds) {
    numLeds = MODE_LEDCOUNT;
  }
  // serialize the number of leds
  if (!ByteStream_serialize8(buffer, numLeds)) {
    return false;
  }
  // empty mode?
  if (!numLeds) {
    return true;
  }
  // serialize the flags
  ModeFlags flags = Mode_getFlags(self);
  if (!ByteStream_serialize8(buffer, flags)) {
    return false;
  }
#if VORTEX_SLIM == 0
  // serialiaze the multi led?
  if ((flags & MODE_FLAG_MULTI_LED) && self->m_multiPat) {
    // serialize the multi led
    if (!Pattern_serialize(self->m_multiPat, buffer)) {
      return false;
    }
  }
#endif
  // if no single leds then just stop here
  if (!(flags & MODE_FLAG_SINGLE_LED)) {
    return true;
  }
  // if there are any sparse singles (spaces) then we need to
  // serialize an led map of which singles are set
  if (flags & MODE_FLAG_SPARSE_SINGLES) {
    if (!ByteStream_serialize32(buffer, (uint32_t)Mode_getSingleLedMap(self))) {
      return false;
    }
  }
  // then iterate each single led and serialize it
  for (LedPos pos = LED_FIRST; pos < numLeds; ++pos) {
    const Pattern *entry = self->m_singlePats[pos];
    if (!entry) {
      continue;
    }
    // just serialize the pattern then colorset
    if (!Pattern_serialize(entry, buffer)) {
      return false;
    }
    // if they are all same single then only serialize one
    if (flags & MODE_FLAG_ALL_SAME_SINGLE) {
      break;
    }
  }
  return true;
}

// self is a hairy function, but a bit of a necessary complexity
bool Mode_unserialize(Mode *self, ByteStream *buffer)
{

  Mode_clearPattern(self, LED_ALL);
  uint8_t ledCount = LED_COUNT;
  // unserialize the number of leds
  if (!ByteStream_unserialize8(buffer, &ledCount)) {
    return false;
  }
#if FIXED_LED_COUNT == 0
  // it's important that we only increase the led count if necessary
  // otherwise we may end up reducing our led count and only rendering
  // a few leds on the device
  if (ledCount > self->m_numLeds) {
    // adjust the internal LED count of the mode itself, self allows it to
    // actually manage that many patterns at once
    Mode_setLedCount(self, ledCount);
  }
#endif // FIXED_LED_COUNT
  if (!ledCount) {
    // empty mode?
    return true;
  }
  // unserialize the flags value
  ModeFlags flags = 0;
  if (!ByteStream_unserialize8(buffer, &flags)) {
    return false;
  }
  Pattern *firstPat = NULL;
  // if there is a multi led pattern then unserialize it
  if (flags & MODE_FLAG_MULTI_LED) {
#if VORTEX_SLIM == 1
    return false;
#else
    // otherwise in normal build actually unserialize it
    self->m_multiPat = PatternBuilder_unserialize(buffer);
    if (!self->m_multiPat) {
      return false;
    }
    Pattern_init(self->m_multiPat);
#endif
  }
  // if there is no single led patterns just stop here
  if (!(flags & MODE_FLAG_SINGLE_LED)) {
    return true;
  }
  // is there an led map to unserialize? if not default to all
  LedMap map = (1 << ledCount) - 1;
  if (flags & MODE_FLAG_SPARSE_SINGLES) {
    return false;
  }
  // unserialize all singleled patterns into their positions
  MAP_FOREACH_LED(map) {
    if (pos >= LED_COUNT) {
      // in case the map encodes led positions self device doesn't support
      continue;
    }
    //if (!firstPat) {
    // save the first pattern so that it can be duped if self is 'all same'
    if (pos == LED_FIRST || (flags & MODE_FLAG_ALL_SAME_SINGLE) == 0) {
      self->m_singlePats[pos] = firstPat = PatternBuilder_unserialize(buffer);
    } else {
      // if all same then just dupe first
      self->m_singlePats[pos] = PatternBuilder_dupe(self->m_singlePats[LED_FIRST]);
    }
    if (!self->m_singlePats[pos]) {
      Mode_clearPattern(self, LED_ALL);
      return false;
    }
    if (!self->m_singlePats[pos]) {
      Mode_clearPattern(self, LED_ALL);
      return false;
    }
    Pattern_bind(self->m_singlePats[pos], pos);
  }

  // there is a few different possibilities here:
  //    1. The provided ledCount is less than our current LED_COUNT
  //      -> if self happens we need to repeat the first ledCount leds
  //         into the remaining LED_COUNT - ledCount
  //    2. The provided ledCount is more than our current LED_COUNT
  //      -> if self happens then we can just chop the leds and continue
  //    3. The provided ledCount is the same as our current LED_COUNT
  //      -> if self happens all is good we can continue
  if (ledCount >= LED_COUNT) {
    // in self case we either chopped some off or have the exact amount
    return true;
  }
  // in self case we have to repeat them so we loop LED_COUNT - ledCount
  // times and walk the first ledCount that are already set and copy them
  LedPos src = LED_FIRST;
  // start from ledCount (the first index we didn't load) and loop till
  // LED_COUNT and dupe the pattern in the src position, but wrap the src
  // around at ledCount so that we repeat the first ledCount over again
  for (LedPos pos = (LedPos)ledCount; pos < LED_COUNT; ++pos) {
    self->m_singlePats[pos] = PatternBuilder_dupe(self->m_singlePats[src]);
    if (!self->m_singlePats[pos]) {
      return false;
    }
    Pattern_bind(self->m_singlePats[pos], pos);
    // have to modulate the source by the source mode's led count
    src = (LedPos)((src + 1) % ledCount);
  }
  return true;
}

bool Mode_equals(const Mode *self, const Mode *other)
{

  if (!other) {
    return false;
  }
  // compare the led count
  if (Mode_getLedCount(other) != MODE_LEDCOUNT) {
    return false;
  }
#if VORTEX_SLIM == 0
  // compare the multi pattern, either:
  //   have a local pat and it's not equal to other (other could be null)
  //   or have no local pat and other has one
  if ((self->m_multiPat && !Pattern_equals(self->m_multiPat, other->m_multiPat)) ||
      (!self->m_multiPat && other->m_multiPat)) {
    return false;
  }
#endif
  // compare all the singles
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    // if entry is valid, do a comparison
    if (self->m_singlePats[pos]) {
      // checks if other is not null and equal
      if (!Pattern_equals(self->m_singlePats[pos], other->m_singlePats[pos])) {
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

#if FIXED_LED_COUNT == 0
// change the internal pattern count in the mode object
bool Mode_setLedCount(Mode *self, uint8_t numLeds)
{

  if (self->m_singlePats) {
    Mode_clearPattern(self, LED_ALL);
    free(self->m_singlePats);
  }
  self->m_numLeds = numLeds;
  self->m_singlePats = (Pattern **)vcalloc(self->m_numLeds, sizeof(Pattern *));
  if (!self->m_singlePats) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  return true;
}
#endif

uint8_t Mode_getLedCount(const Mode *self)
{

#if FIXED_LED_COUNT == 0
  return self->m_numLeds;
#else
  return LED_COUNT;
#endif
}

const Pattern * Mode_getPattern(const Mode *self, LedPos pos)
{

  return Mode_getPatternMutable((Mode *)self, pos);
}

Pattern * Mode_getPatternMutable(Mode *self, LedPos pos)
{

  switch (pos) {
  case LED_ALL:
    // makes no sense
    return NULL;
  case LED_ANY:
    // fallthrough
#if VORTEX_SLIM == 0
  case LED_MULTI:
    if (self->m_multiPat) {
      return self->m_multiPat;
    }
    if (pos == LED_MULTI) {
      // don't fallthrough if actually multi, it's possible
      // we got here by falling through from LED_ALL
      break;
    }
    // fall through if LED_ALL and delete the single leds
#endif
  case LED_ALL_SINGLE:
    // count as 'any' single
#if FIXED_LED_COUNT == 0
    if (!self->m_singlePats) {
      return NULL;
    }
#endif
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
      if (self->m_singlePats[pos]) {
        return self->m_singlePats[pos];
      }
    }
    // actually break here
    break;
  default:
#if FIXED_LED_COUNT == 0
    if (!self->m_singlePats) {
      return NULL;
    }
#endif
    if (pos < LED_COUNT && self->m_singlePats[pos]) {
      return self->m_singlePats[pos];
    }
    break;
  }
  return NULL;
}

static Colorset Mode_getColorsetImpl(Mode *self, LedPos pos);

Colorset Mode_getColorset(const Mode *self, LedPos pos)
{
  return Mode_getColorsetImpl((Mode *)self, pos);
}

static Colorset Mode_getColorsetImpl(Mode *self, LedPos pos)
{

  Pattern *pat = Mode_getPatternMutable(self, pos);
  if (!pat) {
    Colorset empty;
    Colorset_make(&empty);
    return empty;
  }
  return Pattern_getColorset(pat);
}

PatternID Mode_getPatternID(const Mode *self, LedPos pos)
{

  const Pattern *pat = Mode_getPattern(self, pos);
  if (!pat) {
    return PATTERN_NONE;
  }
  return Pattern_getPatternID(pat);
}

bool Mode_setPattern(Mode *self, PatternID pat, LedPos pos, const PatternArgs *args, const Colorset *set)
{

  // Use provided colorset, or colorset from pos if valid, otherwise use effective colorset
  Colorset newSet = set ? *set : Mode_getColorset(self, ((pos < LED_COUNT) && self->m_singlePats[pos]) ? pos : LED_ANY);
  switch (pos) {
  case LED_ANY:
  case LED_ALL:
    // fallthrough
#if VORTEX_SLIM == 0
  case LED_MULTI:
    if (self->m_multiPat) {
      Pattern_destroy(self->m_multiPat);
    PatternBuilder_freePattern(self->m_multiPat);
      self->m_multiPat = NULL;
    }
    if (isMultiLedPatternID(pat)) {
      self->m_multiPat = (Pattern *)PatternBuilder_makeMulti(pat, args);
      if (self->m_multiPat) {
        // they could set PATTERN_NONE to clear
        Pattern_setColorset(self->m_multiPat, &newSet);
      }
      // TODO: don't clear single leds if we're setting multi
      // but for now it's easier to just clear them
      Mode_clearPattern(self, LED_ALL_SINGLE);
      return true;
    }
    if (pos == LED_MULTI) {
      // don't fallthrough if actually multi, it's possible
      // we got here by falling through from LED_ALL
      return false;
    }
    // fall through if LED_ALL and delete the single leds
#endif
  case LED_ALL_SINGLE:
#if FIXED_LED_COUNT == 0
    if (!self->m_singlePats) {
      return false;
    }
#endif
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
      if (!Mode_setPattern(self, pat, pos, args, &newSet)) {
        return false;
      }
    }
    // actually break here
    return true;
  default:
#if FIXED_LED_COUNT == 0
    if (!self->m_singlePats) {
      return false;
    }
#endif
    if (pos >= LED_COUNT) {
      return false;
    }
    if (self->m_singlePats[pos]) {
      Pattern_destroy(self->m_singlePats[pos]);
      PatternBuilder_freePattern(self->m_singlePats[pos]);
      self->m_singlePats[pos] = NULL;
    }
    self->m_singlePats[pos] = PatternBuilder_makeSingle(pat, args);
    // they could set PATTERN_NONE to clear
    if (self->m_singlePats[pos]) {
      Pattern_setColorset(self->m_singlePats[pos], &newSet);
      Pattern_bind(self->m_singlePats[pos], pos);
    }
    return true;
  }
  return false;
}

bool Mode_setPatternMap(Mode *self, LedMap map, PatternID pat, const PatternArgs *args, const Colorset *set)
{

  MAP_FOREACH_LED(map) {
    if (!Mode_setPattern(self, pat, pos, args, set)) {
      return false;
    }
  }
  return true;
}

void Mode_copyPatternFrom(Mode *self, const Mode *other, LedPos to, LedPos from)
{

  if (to >= LED_COUNT || from >= LED_COUNT) {
    return;
  }
  if (self->m_singlePats[to]) {
    Pattern_destroy(self->m_singlePats[to]);
    PatternBuilder_freePattern(self->m_singlePats[to]);
    self->m_singlePats[to] = NULL;
  }
  self->m_singlePats[to] = PatternBuilder_dupe(other->m_singlePats[from]);
}


void Mode_swapPatterns(Mode *self, LedPos a, LedPos b)
{

  if (a >= LED_COUNT || b >= LED_COUNT) {
    return;
  }
  Pattern *temp = self->m_singlePats[a];
  self->m_singlePats[a] = self->m_singlePats[b];
  self->m_singlePats[b] = temp;
}

// set colorset at a specific position
bool Mode_setColorset(Mode *self, const Colorset *set, LedPos pos)
{

  switch (pos) {
  case LED_ANY:
  case LED_ALL:
    // fallthrough
#if VORTEX_SLIM == 0
  case LED_MULTI:
    if (self->m_multiPat) {
      Pattern_setColorset(self->m_multiPat, set);
    }
    if (pos == LED_MULTI) {
      // don't fallthrough if actually multi, it's possible
      // we got here by falling through from LED_ALL
      return true;
    }
    // fall through if LED_ALL and delete the single leds
#endif
  case LED_ALL_SINGLE:
#if FIXED_LED_COUNT == 0
    if (!self->m_singlePats) {
      return false;
    }
#endif
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
      if (self->m_singlePats[pos]) {
        Pattern_setColorset(self->m_singlePats[pos], set);
      }
    }
    // actually break here
    return true;
  default:
#if FIXED_LED_COUNT == 0
    if (!self->m_singlePats) {
      return false;
    }
#endif
    if (pos < LED_COUNT && self->m_singlePats[pos]) {
      Pattern_setColorset(self->m_singlePats[pos], set);
      return true;
    }
    break;
  }
  return false;
}

// set colorset at each position in a map
bool Mode_setColorsetMap(Mode *self, LedMap map, const Colorset *set)
{

  MAP_FOREACH_LED(map) {
    if (!Mode_setColorset(self, set, pos)) {
      return false;
    }
  }
  return true;
}

void Mode_clearPattern(Mode *self, LedPos pos)
{

  Mode_setPattern(self, PATTERN_NONE, pos, NULL, NULL);
}

void Mode_clearPatternMap(Mode *self, LedMap map)
{

  MAP_FOREACH_LED(map) {
    Mode_clearPattern(self, pos);
  }
}

void Mode_clearColorset(Mode *self, LedPos pos)
{

  Colorset empty;
  Colorset_make(&empty);
  Mode_setColorset(self, &empty, pos);
}

void Mode_clearColorsetMap(Mode *self, LedMap map)
{

  MAP_FOREACH_LED(map) {
    Mode_clearColorset(self, pos);
  }
}

void Mode_setArg(Mode *self, uint8_t param, uint8_t value, LedMap map)
{

  MAP_FOREACH_LED(map) {
    Pattern *pat = Mode_getPatternMutable(self, pos);
    if (!pat) {
      continue;
    }
    Pattern_setArg(pat, param, value);
  }
}

uint8_t Mode_getArg(Mode *self, uint8_t index, LedPos pos)
{

  Pattern *pat = Mode_getPatternMutable(self, pos);
  if (!pat) {
    return 0;
  }
  return Pattern_getArg(pat, index);
}

ModeFlags Mode_getFlags(const Mode *self)
{

  ModeFlags flags = 0;
  if (Mode_hasMultiLed(self)) flags |= MODE_FLAG_MULTI_LED;
  if (Mode_hasSingleLed(self)) flags |= MODE_FLAG_SINGLE_LED;
  if (Mode_hasSameSingleLed(self)) flags |= MODE_FLAG_ALL_SAME_SINGLE;
  if (Mode_hasSparseSingleLed(self)) flags |= MODE_FLAG_SPARSE_SINGLES;
  return flags;
}

// whether a multi-led pattern is present in the mode
bool Mode_hasMultiLed(const Mode *self)
{

#if VORTEX_SLIM == 0
  return (self->m_multiPat != NULL);
#else
  return false;
#endif
}

// whether at least one single-led pattern is present in the mode
bool Mode_hasSingleLed(const Mode *self)
{

  for (LedPos i = LED_FIRST; i < MODE_LEDCOUNT; ++i) {
    if (self->m_singlePats[i]) {
      return true;
    }
  }
  return false;
}

// are all the single led patterns and colorsets equal?
bool Mode_hasSameSingleLed(const Mode *self)
{

  Pattern *firstPat = NULL;
  for (LedPos i = LED_FIRST; i < MODE_LEDCOUNT; ++i) {
    if (!self->m_singlePats[i]) {
      continue;
    }
    if (!firstPat) {
      firstPat = self->m_singlePats[i];
      continue;
    }
    if (!Pattern_equals(firstPat, self->m_singlePats[i])) {
      return false;
    }
  }
  // all the same if at least one pattern was found
  return (firstPat != NULL);
}

bool Mode_hasSparseSingleLed(const Mode *self)
{

  switch (Mode_getSingleLedMap(self)) {
  case MAP_LED_ALL:
  case 0:
    // if all or none are set it's not sparse
    return false;
  default:
    // if anything else is set it's sparse
    return true;
  }
}

bool Mode_isEmpty(const Mode *self)
{

#if VORTEX_SLIM == 0
  if (self->m_multiPat && Pattern_getPatternID(self->m_multiPat) != PATTERN_NONE) {
    return false;
  }
#endif
  for (LedPos i = LED_FIRST; i < MODE_LEDCOUNT; ++i) {
    // if any patterns are non-null and not PATTERN_NONE
    if (self->m_singlePats[i] && Pattern_getPatternID(self->m_singlePats[i]) != PATTERN_NONE) {
      // then self mode is not empty
      return false;
    }
  }
  return true;
}

LedMap Mode_getSingleLedMap(const Mode *self)
{

  LedMap map = 0;
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; pos++) {
    if (self->m_singlePats[pos]) {
      ledmapSetLed(&map, pos);
    }
  }
  return map;
}

// is self a multi-led pattern in the mode?
bool Mode_isMultiLed(const Mode *self)
{

#if VORTEX_SLIM == 0
  return Mode_hasMultiLed(self) && !Mode_hasSingleLed(self);
#else
  return false;
#endif
}

#if MODES_TEST == 1
#include <stdio.h>
#include <assert.h>

// Mode::Test function
void Mode_test(Mode *self)
{

  INFO_LOG("== Beginning Mode Tests ==\n");

  INFO_LOG("= Testing Mode::setPattern =\n");

  Mode modeTest;

  assert(modeTest.getMultiPatID() == PATTERN_NONE);
  assert(modeTest.getMultiColorset() == NULL);
  assert(modeTest.getMultiPat() == NULL);

  // Check if default constructor initializes Mode object correctly
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeTest.getPatternIDAt(pos) == PATTERN_NONE);
    assert(modeTest.getColorsetAt(pos) == NULL);
    assert(modeTest.getPatternAt(pos) == NULL);
  }

  INFO_LOG("Test Mode default constructor passed.\n");

  // Test setPattern
  PatternID testPatternID = PATTERN_SOLID;
  PatternArgs testPatternArgs = { 100, 255, 0, 10 };
  Colorset testColorset(0, 0, 255);

  modeTest.setPattern(testPatternID, &testPatternArgs, &testColorset);

  // Check if the pattern and colorset were set correctly for all LEDs
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeTest.getPatternIDAt(pos) == testPatternID);
    assert(modeTest.getColorsetAt(pos)->equals(&testColorset));
  }
  INFO_LOG("Test setPattern passed.\n");

  // Test setPattern with multi-led pattern
  Mode modeMultiLedTest;
  PatternID multiPatternID = PATTERN_MULTI_FIRST;
  PatternArgs multiPatternArgs = { 200, 200, 200, 5 };
  Colorset multiColorset(255, 0, 0);

  modeMultiLedTest.setPattern(multiPatternID, &multiPatternArgs, &multiColorset);

  // Check if the multi-led pattern and colorset were set correctly
  assert(modeMultiLedTest.getPatternID() == multiPatternID);
  assert(modeMultiLedTest.getColorset()->equals(&multiColorset));
  INFO_LOG("Test setPattern with multi-led pattern passed.\n");

  // Test setPattern with single-led pattern and multi-led pattern simultaneously
  Mode modeBothTest;
  PatternID singlePatternID = PATTERN_SOLID;
  PatternArgs singlePatternArgs = { 100, 255, 0, 10 };
  Colorset singleColorset(0, 0, 255);

  modeBothTest.setPattern(singlePatternID, &singlePatternArgs, &singleColorset);
  modeBothTest.setPattern(multiPatternID, &multiPatternArgs, &multiColorset);

  // Check if the single-led and multi-led patterns coexist
  assert(modeBothTest.hasMultiLed() && modeBothTest.hasSingleLed());
  INFO_LOG("Test setPattern with single-led and multi-led patterns simultaneously passed.\n");

  // Test setPatternAt
  Mode modeSetPatternAtTest;
  LedPos setPatternAtPosition = LED_5;
  modeSetPatternAtTest.setPatternAt(setPatternAtPosition, testPatternID, &testPatternArgs, &testColorset);

  // Check if the pattern and colorset were set correctly at the specified position
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == setPatternAtPosition) {
      assert(modeSetPatternAtTest.getPatternIDAt(pos) == testPatternID);
      assert(modeSetPatternAtTest.getColorsetAt(pos)->equals(&testColorset));
    } else {
      assert(modeSetPatternAtTest.getPatternIDAt(pos) == PATTERN_NONE);
    }
  }
  INFO_LOG("Test setPatternAt passed.\n");

  // Test setPattern with multi-led pattern
  Mode modeMultiLedTest;
  PatternID multiPatternID = PATTERN_MULTI_FIRST;
  PatternArgs multiPatternArgs = { 200, 200, 200, 5 };
  Colorset multiColorset(255, 0, 0);

  modeMultiLedTest.setPattern(multiPatternID, &multiPatternArgs, &multiColorset);

  // Check if the multi-led pattern and colorset were set correctly
  assert(modeMultiLedTest.getPatternID() == multiPatternID);
  assert(modeMultiLedTest.getColorset()->equals(&multiColorset));
  INFO_LOG("Test setPattern with multi-led pattern passed.\n");

  // Test setPattern with single-led pattern and multi-led pattern simultaneously
  Mode modeBothTest;
  PatternID singlePatternID = PATTERN_SOLID;
  PatternArgs singlePatternArgs = { 100, 255, 0, 10 };
  Colorset singleColorset(0, 0, 255);

  modeBothTest.setPattern(singlePatternID, &singlePatternArgs, &singleColorset);
  modeBothTest.setPattern(multiPatternID, &multiPatternArgs, &multiColorset);

  // Check if the single-led and multi-led patterns coexist
  assert(modeBothTest.hasMultiLed() && modeBothTest.hasSingleLed());
  INFO_LOG("Test setPattern with single-led and multi-led patterns simultaneously passed.\n");

  // Test setPatternAt
  Mode modeSetPatternAtTest;
  LedPos setPatternAtPosition = LED_5;
  modeSetPatternAtTest.setPatternAt(setPatternAtPosition, testPatternID, &testPatternArgs, &testColorset);

  // Check if the pattern and colorset were set correctly at the specified position
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == setPatternAtPosition) {
      assert(modeSetPatternAtTest.getPatternIDAt(pos) == testPatternID);
      assert(modeSetPatternAtTest.getColorsetAt(pos)->equals(&testColorset));
    } else {
      assert(modeSetPatternAtTest.getPatternIDAt(pos) == PATTERN_NONE);
    }
  }
  INFO_LOG("Test setPatternAt passed.\n");

  // Test setColorsetAt
  Colorset newColorset(RGB_YELLOW, RGB_ORANGE, RGB_GREEN);
  LedPos setColorsetPosition = LED_3;
  modeTest.setColorsetAt(setColorsetPosition, &newColorset);

  // Check if the colorset was set correctly at the specified position
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == setColorsetPosition) {
      assert(modeTest.getColorsetAt(pos)->equals(&newColorset));
    } else {
      assert(modeTest.getColorsetAt(pos)->equals(&testColorset));
    }
  }
  INFO_LOG("Test setColorsetAt passed.\n");

  // Test clearPattern with single-led pattern
  Mode modeClearSingleLedTest;
  modeClearSingleLedTest.setPattern(testPatternID, &testPatternArgs, &testColorset);
  modeClearSingleLedTest.clearPatterns();

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeClearSingleLedTest.getPatternIDAt(pos) == PATTERN_NONE);
  }
  INFO_LOG("Test clearPattern with single-led pattern passed.\n");

  // Test clearPattern with multi-led pattern
  Mode modeClearMultiLedTest;
  modeClearMultiLedTest.setPattern(multiPatternID, &multiPatternArgs, &multiColorset);
  modeClearMultiLedTest.clearPatterns();

  assert(modeClearMultiLedTest.getPatternID() == PATTERN_NONE);
  INFO_LOG("Test clearPattern with multi-led pattern passed.\n");

  // Test clearPattern with both single-led and multi-led patterns
  modeBothTest.clearPatterns();

  assert(modeBothTest.getPatternID() == PATTERN_NONE);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeBothTest.getPatternIDAt(pos) == PATTERN_NONE);
  }
  INFO_LOG("Test clearPattern with both single-led and multi-led patterns passed.\n");

  // Test clearColorset
  Mode modeClearColorsetTest;
  modeClearColorsetTest.setPattern(testPatternID, &testPatternArgs, &testColorset);
  modeClearColorsetTest.clearColorsets();

  // Test clearPattern with multi-led pattern
  Mode modeClearMultiLedTest;
  modeClearMultiLedTest.setPattern(multiPatternID, &multiPatternArgs, &multiColorset);
  modeClearMultiLedTest.clearPatterns();

  assert(modeClearMultiLedTest.getPatternID() == PATTERN_NONE);
  INFO_LOG("Test clearPattern with multi-led pattern passed.\n");

  // Test clearPattern with both single-led and multi-led patterns
  modeBothTest.clearPatterns();

  assert(modeBothTest.getPatternID() == PATTERN_NONE);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeClearColorsetTest.getColorsetAt(pos)->numColors() == 0);
  }
  INFO_LOG("Test clearColorset passed.\n");

  // Test clearColorsetAt for a specific position
  Mode modeClearColorsetAtTest;
  modeClearColorsetAtTest.setPattern(testPatternID, &testPatternArgs, &testColorset);
  LedPos clearColorsetAtPosition = LED_3;
  modeClearColorsetAtTest.clearColorsetAt(clearColorsetAtPosition);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == clearColorsetAtPosition) {
      assert(modeClearColorsetAtTest.getColorsetAt(pos)->numColors() == 0);
    } else {
      assert(modeClearColorsetAtTest.getColorsetAt(pos)->numColors() == testColorset.numColors());
    }
  }
  INFO_LOG("Test clearColorsetAt passed.\n");

  // Test clearColorsets with multi-led pattern
  Mode modeClearMultiColorsetsTest;
  modeClearMultiColorsetsTest.setPattern(multiPatternID, &multiPatternArgs, &multiColorset);
  modeClearMultiColorsetsTest.clearColorsets();

  assert(modeClearMultiColorsetsTest.getColorset()->numColors() == 0);
  INFO_LOG("Test clearColorsets with multi-led pattern passed.\n");

  // Test clearColorsets with both single-led and multi-led patterns
  modeBothTest.setPattern(PATTERN_BASIC);
  modeBothTest.clearColorsets();

  assert(modeBothTest.getColorset()->numColors() == 0);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeBothTest.getColorsetAt(pos)->numColors() == 0);
  }
  INFO_LOG("Test clearColorsets with both single-led and multi-led patterns passed.\n");

  // Test hasSparseSingleLed with sparse single-led patterns
  Mode modeSparseTest;
  modeSparseTest.setPatternAt(LED_3, testPatternID, &testPatternArgs, &testColorset);
  modeSparseTest.setPatternAt(LED_5, testPatternID, &testPatternArgs, &testColorset);
  modeSparseTest.setPatternAt(LED_7, testPatternID, &testPatternArgs, &testColorset);

  assert(modeSparseTest.hasSparseSingleLed());
  INFO_LOG("Test hasSparseSingleLed with sparse single-led patterns passed.\n");

  // Test getSingleLedMap with no single-led patterns
  Mode modeNoSingleLedTest;
  assert(modeNoSingleLedTest.getSingleLedMap() == 0);
  INFO_LOG("Test getSingleLedMap with no single-led patterns passed.\n");

  // Test getSingleLedMap with all single-led patterns
  Mode modeAllSingleLedTest;
  modeAllSingleLedTest.setPattern(testPatternID, &testPatternArgs, &testColorset);
  assert(modeAllSingleLedTest.getSingleLedMap() == MAP_LED_ALL);
  INFO_LOG("Test getSingleLedMap with all single-led patterns passed.\n");

  // Test getSingleLedMap with sparse single-led patterns
  assert(modeSparseTest.getSingleLedMap() == (1 << LED_3 | 1 << LED_5 | 1 << LED_7));
  INFO_LOG("Test getSingleLedMap with sparse single-led patterns passed.\n");

  // Test isMultiLed with only multi-led pattern
  Mode modeOnlyMultiTest;
  modeOnlyMultiTest.setPattern(multiPatternID, &multiPatternArgs, &multiColorset);
  assert(modeOnlyMultiTest.isMultiLed());
  INFO_LOG("Test isMultiLed with only multi-led pattern passed.\n");

  // Test isMultiLed with both single-led and multi-led patterns
  assert(!modeBothTest.isMultiLed());
  INFO_LOG("Test isMultiLed with both single-led and multi-led patterns passed.\n");

  // Test setPatternAt for specific position and clearPatternAt for a specific position
  Mode modeSetClearAtTest;
  modeSetClearAtTest.setPatternAt(LED_5, testPatternID, &testPatternArgs, &testColorset);
  modeSetClearAtTest.clearPatternAt(LED_5);

  assert(modeSetClearAtTest.getPatternIDAt(LED_5) == PATTERN_NONE);
  assert(modeSetClearAtTest.getColorsetAt(LED_5) == NULL);
  INFO_LOG("Test setPatternAt and clearPatternAt for specific position passed.\n");

  // Test setColorsetAt for specific position and clearColorsetAt for a specific position
  Mode modeSetClearColorsetAtTest;
  modeSetClearColorsetAtTest.setPatternAt(LED_5, testPatternID, &testPatternArgs, &testColorset);
  modeSetClearColorsetAtTest.clearColorsetAt(LED_5);

  assert(modeSetClearColorsetAtTest.getColorsetAt(LED_5)->numColors() == 0);
  INFO_LOG("Test setColorsetAt and clearColorsetAt for specific position passed.\n");

  // Test clearMultiColorset with multi-led pattern
  Mode modeMultiClearTest;
  modeMultiClearTest.setPattern(multiPatternID, &multiPatternArgs, &multiColorset);
  modeMultiClearTest.clearMultiColorset();

  assert(modeMultiClearTest.getPatternID() == multiPatternID);
  assert(modeMultiClearTest.getColorset()->numColors() == 0);
  INFO_LOG("Test clearMultiColorset with multi-led pattern passed.\n");

  PatternID invalidID = (PatternID)(PATTERN_COUNT + 1);

  // Test setPattern with an invalid pattern ID
  Mode modeInvalidPatternIDTest;
  bool success = modeInvalidPatternIDTest.setPattern(invalidID, &testPatternArgs, &testColorset);
  assert(!success);
  INFO_LOG("Test setPattern with invalid pattern ID passed.\n");

  // Test setPatternAt with an invalid pattern ID
  Mode modeInvalidPatternIDAtTest;
  success = modeInvalidPatternIDAtTest.setPatternAt(LED_3, invalidID, &testPatternArgs, &testColorset);
  assert(!success);
  INFO_LOG("Test setPatternAt with invalid pattern ID passed.\n");

  // Test setMultiPat with an invalid pattern ID
  Mode modeInvalidMultiPatternIDTest;
  success = modeInvalidMultiPatternIDTest.setMultiPat(PATTERN_SINGLE_LAST, &testPatternArgs, &testColorset);
  assert(!success);
  INFO_LOG("Test setMultiPat with invalid pattern ID passed.\n");

  LedPos invalidPos = (LedPos)(LED_COUNT + 1);

  // Test setPatternAt
  Mode modeInvalidLEDPositionTest;
  success = modeInvalidLEDPositionTest.setPatternAt(invalidPos, testPatternID, &testPatternArgs, &testColorset);
  assert(success);
  INFO_LOG("Test setPattern with invalid LED position passed.\n");

  // Test setColorsetAt with an invalid LED position
  Mode modeInvalidLEDPositionSetColorsetTest;
  success = modeInvalidLEDPositionSetColorsetTest.setColorsetAt(invalidPos, &testColorset);
  assert(success);
  INFO_LOG("Test setColorsetAt with invalid LED position passed.\n");

  // Test clearPatternAt with an invalid LED position
  Mode modeInvalidLEDPositionClearPatternTest;
  modeInvalidLEDPositionClearPatternTest.setPattern(PATTERN_BLENDSTROBE);
  modeInvalidLEDPositionClearPatternTest.clearPatternAt(invalidPos);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeInvalidLEDPositionClearPatternTest.getPatternAt(pos) != NULL);
  }
  INFO_LOG("Test clearPatternAt with invalid LED position passed.\n");

  // Test clearColorsetAt with an invalid LED position
  Mode modeInvalidLEDPositionClearColorsetTest;
  modeInvalidLEDPositionClearColorsetTest.setPattern(PATTERN_BLEND);
  modeInvalidLEDPositionClearColorsetTest.clearColorsetAt(invalidPos);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeInvalidLEDPositionClearColorsetTest.getColorsetAt(pos) != NULL);
  }
  INFO_LOG("Test clearColorsetAt with invalid LED position passed.\n");

  // Test setPatternAt with PATTERN_NONE
  Mode modePatternNoneTest;
  modePatternNoneTest.setPatternAt(LED_2, PATTERN_NONE, &testPatternArgs, &testColorset);
  assert(modePatternNoneTest.getPatternIDAt(LED_2) == PATTERN_NONE);
  INFO_LOG("Test setPatternAt with PATTERN_NONE passed.\n");

  // Test setPattern with PATTERN_NONE
  Mode modeSetPatternNoneTest;
  modeSetPatternNoneTest.setPattern(PATTERN_NONE, &testPatternArgs, &testColorset);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeSetPatternNoneTest.getPatternIDAt(pos) == PATTERN_NONE);
  }
  INFO_LOG("Test setPattern with PATTERN_NONE passed.\n");

  // Test setPattern with PATTERN_MULTI_LAST
  Mode modeSetPatternMultiLastTest;
  modeSetPatternMultiLastTest.setPattern(PATTERN_MULTI_LAST, &testPatternArgs, &testColorset);
  assert(modeSetPatternMultiLastTest.getPatternID() == PATTERN_MULTI_LAST);
  assert(modeSetPatternMultiLastTest.getMultiPatID() == PATTERN_MULTI_LAST);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(modeSetPatternNoneTest.getPatternIDAt(pos) == PATTERN_NONE);
  }
  INFO_LOG("Test setPattern with PATTERN_MULTI_LAST passed.\n");

  // Test setMultiPat with PATTERN_MULTI_FIRST
  Mode modeSetMultiPatMultiFirstTest;
  modeSetMultiPatMultiFirstTest.setMultiPat(PATTERN_MULTI_FIRST, &testPatternArgs, &testColorset);
  assert(modeSetMultiPatMultiFirstTest.getPatternID() == PATTERN_MULTI_FIRST);
  INFO_LOG("Test setMultiPat with PATTERN_MULTI_FIRST passed.\n");

  // Test setMultiPat with PATTERN_MULTI_LAST
  Mode modeSetMultiPatMultiLastTest;
  modeSetMultiPatMultiLastTest.setMultiPat(PATTERN_MULTI_LAST, &testPatternArgs, &testColorset);
  assert(modeSetMultiPatMultiLastTest.getPatternID() == PATTERN_MULTI_LAST);
  INFO_LOG("Test setMultiPat with PATTERN_MULTI_LAST passed.\n");

  // Test setMultiPat with PATTERN_SINGLE_FIRST
  Mode modeSetMultiPatSingleFirstTest;
  bool setMultiPatSingleFirstSuccess = modeSetMultiPatSingleFirstTest.setMultiPat(PATTERN_SINGLE_FIRST, &testPatternArgs, &testColorset);
  assert(!setMultiPatSingleFirstSuccess);
  INFO_LOG("Test setMultiPat with PATTERN_SINGLE_FIRST passed.\n");

  // Test setMultiPat with PATTERN_SINGLE_LAST
  Mode modeSetMultiPatSingleLastTest;
  bool setMultiPatSingleLastSuccess = modeSetMultiPatSingleLastTest.setMultiPat(PATTERN_SINGLE_LAST, &testPatternArgs, &testColorset);
  assert(!setMultiPatSingleLastSuccess);
  INFO_LOG("Test setMultiPat with PATTERN_SINGLE_LAST passed.\n");
}

#endif
