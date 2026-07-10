#include "Mode.h"

#include "../Patterns/Patterns.h"
#include "../Patterns/PatternArgs.h"
#include "../Patterns/Pattern.h"
#include "../Patterns/PatternBuilder.h"
#include "../Colors/Colorset.h"
#include "../Serial/ByteStream.h"
#include "../Leds/Leds.h"
#include "../Memory/Memory.h"
#include "../Log/Log.h"
#include "../VortexEngine.h"

void Mode_initFromEntry(Mode *self, const struct DefaultModeEntry *entry)
{
  Mode_init(self);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    Colorset set;
    Colorset_initFromU32s(&set, entry->leds[pos].numColors, entry->leds[pos].cols);
    Mode_setPattern(self, entry->leds[pos].patternID, pos, NULL, &set);
  }
}

#if FIXED_LED_COUNT == 0
#define MODE_LEDCOUNT self->m_numLeds

static void Mode_initInternal(Mode *self, uint8_t numLeds)
{
#if VORTEX_SLIM == 0
  self->m_multiPat = NULL;
#endif
  self->m_numLeds = numLeds;
  self->m_singlePats = NULL;
  Mode_setLedCount(self, self->m_numLeds);
}

void Mode_init(Mode *self)
{
  Mode_initInternal(self, LED_COUNT);
}
#else
#define MODE_LEDCOUNT LED_COUNT

void Mode_init(Mode *self)
{
#if VORTEX_SLIM == 0
  self->m_multiPat = NULL;
#endif
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    self->m_singlePats[i] = NULL;
  }
}
#endif

void Mode_initFromID(Mode *self, PatternID id, const Colorset *set)
{
  Mode_init(self);
  Mode_setPattern(self, id, LED_ANY, NULL, set);
}

void Mode_initFromIDArgsSet(Mode *self, PatternID id, const PatternArgs *args, const Colorset *set)
{
  Mode_init(self);
  Mode_setPattern(self, id, LED_ANY, args, set);
}

void Mode_initFromIDArgsSetPtr(Mode *self, PatternID id, const PatternArgs *args, const Colorset *set)
{
  Mode_init(self);
  Mode_setPattern(self, id, LED_ANY, args, set);
}

void Mode_initFromOther(Mode *self, const Mode *other)
{
  Mode_init(self);
  if (!other) {
    return;
  }
  Mode_assign(self, other);
}

void Mode_initCopy(Mode *self, const Mode *other)
{
  Mode_init(self);
  Mode_assign(self, other);
}

void Mode_cleanup(Mode *self)
{
  Mode_clearPattern(self, LED_ALL);
#if FIXED_LED_COUNT == 0
  vfree(self->m_singlePats);
#endif
}

void Mode_assign(Mode *self, const Mode *other)
{
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

bool Mode_notEquals(const Mode *self, const Mode *other)
{
  return !Mode_equals(self, other);
}

void Mode_initMode(Mode *self)
{
#if VORTEX_SLIM == 0
  if (self->m_multiPat) {
    Pattern_init(self->m_multiPat, NULL);
  }
#endif
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    Pattern *entry = self->m_singlePats[pos];
    if (!entry) {
      continue;
    }
    Pattern_init(entry, NULL);
  }
}

void Mode_play(Mode *self)
{
#if VORTEX_SLIM == 0
  if (self->m_multiPat) {
    Pattern_play(self->m_multiPat);
  }
#endif
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    Pattern *entry = self->m_singlePats[pos];
    if (!entry) {
#if VORTEX_SLIM == 0
      if (!self->m_multiPat) {
        Leds_clearIndex(pos);
      }
#else
      Leds_clearIndex(pos);
#endif
      continue;
    }
    Pattern_play(entry);
  }
}

bool Mode_saveToBuffer(const Mode *self, ByteStream *saveBuffer, uint8_t numLeds)
{
  VortexEngine_serializeVersion(saveBuffer);
  Mode_serialize(self, saveBuffer, numLeds);
  DEBUG_LOGF("Serialized mode, uncompressed size: %u", ByteStream_size(saveBuffer));
  return ByteStream_compress(saveBuffer);
}

bool Mode_loadFromBuffer(Mode *self, ByteStream *saveBuffer)
{
  if (!ByteStream_decompress(saveBuffer)) {
    return false;
  }
  ByteStream_resetUnserializer(saveBuffer);
  uint8_t major = 0;
  uint8_t minor = 0;
  if (!ByteStream_unserialize8(saveBuffer, &major)) {
    return false;
  }
  if (!ByteStream_unserialize8(saveBuffer, &minor)) {
    return false;
  }
  if (!VortexEngine_checkVersion(major, minor)) {
    ERROR_LOGF("Incompatible savefile version: %u.%u", major, minor);
    return false;
  }
  if (!Mode_unserialize(self, saveBuffer)) {
    return false;
  }
  Mode_initMode(self);
  return true;
}

bool Mode_serialize(const Mode *self, ByteStream *buffer, uint8_t numLeds)
{
  if (!numLeds) {
    numLeds = MODE_LEDCOUNT;
  }
  if (!ByteStream_serialize8(buffer, numLeds)) {
    return false;
  }
  if (!numLeds) {
    return true;
  }
  ModeFlags flags = Mode_getFlags(self);
  if (!ByteStream_serialize8(buffer, flags)) {
    return false;
  }
#if VORTEX_SLIM == 0
  if ((flags & MODE_FLAG_MULTI_LED) && self->m_multiPat) {
    if (!Pattern_serialize(self->m_multiPat, buffer)) {
      return false;
    }
  }
#endif
  if (!(flags & MODE_FLAG_SINGLE_LED)) {
    return true;
  }
  if (flags & MODE_FLAG_SPARSE_SINGLES) {
    if (!ByteStream_serialize32(buffer, (uint32_t)Mode_getSingleLedMap(self))) {
      return false;
    }
  }
  for (LedPos pos = LED_FIRST; pos < numLeds; ++pos) {
    const Pattern *entry = self->m_singlePats[pos];
    if (!entry) {
      continue;
    }
    if (!Pattern_serialize(entry, buffer)) {
      return false;
    }
    if (flags & MODE_FLAG_ALL_SAME_SINGLE) {
      break;
    }
  }
  return true;
}

bool Mode_unserialize(Mode *self, ByteStream *buffer)
{
  Mode_clearPattern(self, LED_ALL);
  uint8_t ledCount = LED_COUNT;
  if (!ByteStream_unserialize8(buffer, &ledCount)) {
    return false;
  }
#if FIXED_LED_COUNT == 0
  if (ledCount > self->m_numLeds) {
    Mode_setLedCount(self, ledCount);
  }
#endif
  if (!ledCount) {
    return true;
  }
  ModeFlags flags = 0;
  if (!ByteStream_unserialize8(buffer, &flags)) {
    return false;
  }
  Pattern *firstPat = NULL;
  if (flags & MODE_FLAG_MULTI_LED) {
#if VORTEX_SLIM == 1
    return false;
#else
    self->m_multiPat = PatternBuilder_unserialize(buffer);
    if (!self->m_multiPat) {
      return false;
    }
    Pattern_init(self->m_multiPat, NULL);
#endif
  }
  if (!(flags & MODE_FLAG_SINGLE_LED)) {
    return true;
  }
  LedMap map = (1 << ledCount) - 1;
  if (flags & MODE_FLAG_SPARSE_SINGLES) {
    return false;
  }
  MAP_FOREACH_LED(map) {
    if (pos >= LED_COUNT) {
      continue;
    }
    if (pos == LED_FIRST || (flags & MODE_FLAG_ALL_SAME_SINGLE) == 0) {
      self->m_singlePats[pos] = firstPat = PatternBuilder_unserialize(buffer);
    } else {
      self->m_singlePats[pos] = PatternBuilder_dupe(self->m_singlePats[LED_FIRST]);
    }
    if (!self->m_singlePats[pos]) {
      Mode_clearPattern(self, LED_ALL);
      return false;
    }
    Pattern_bind(self->m_singlePats[pos], pos);
  }
  if (ledCount >= LED_COUNT) {
    return true;
  }
  LedPos src = LED_FIRST;
  for (LedPos pos = (LedPos)ledCount; pos < LED_COUNT; ++pos) {
    self->m_singlePats[pos] = PatternBuilder_dupe(self->m_singlePats[src]);
    if (!self->m_singlePats[pos]) {
      return false;
    }
    Pattern_bind(self->m_singlePats[pos], pos);
    src = (LedPos)((src + 1) % ledCount);
  }
  return true;
}

bool Mode_equals(const Mode *self, const Mode *other)
{
  if (!other) {
    return false;
  }
  if (Mode_getLedCount(other) != MODE_LEDCOUNT) {
    return false;
  }
#if VORTEX_SLIM == 0
  if ((self->m_multiPat && !Pattern_equals(self->m_multiPat, other->m_multiPat)) ||
      (!self->m_multiPat && other->m_multiPat)) {
    return false;
  }
#endif
  for (LedPos pos = LED_FIRST; pos < MODE_LEDCOUNT; ++pos) {
    if (self->m_singlePats[pos]) {
      if (!Pattern_equals(self->m_singlePats[pos], other->m_singlePats[pos])) {
        return false;
      }
      continue;
    }
    if (other->m_singlePats[pos]) {
      return false;
    }
  }
  return true;
}

#if FIXED_LED_COUNT == 0
bool Mode_setLedCount(Mode *self, uint8_t numLeds)
{
  if (self->m_singlePats) {
    Mode_clearPattern(self, LED_ALL);
    vfree(self->m_singlePats);
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

const Pattern *Mode_getPatternConst(const Mode *self, LedPos pos)
{
  return Mode_getPattern((Mode *)self, pos);
}

Pattern *Mode_getPattern(Mode *self, LedPos pos)
{
  switch (pos) {
  case LED_ALL:
    return NULL;
  case LED_ANY:
#if VORTEX_SLIM == 0
  case LED_MULTI:
    if (self->m_multiPat) {
      return self->m_multiPat;
    }
    if (pos == LED_MULTI) {
      break;
    }
#endif
  case LED_ALL_SINGLE:
#if FIXED_LED_COUNT == 0
    if (!self->m_singlePats) {
      return NULL;
    }
#endif
    for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
      if (self->m_singlePats[p]) {
        return self->m_singlePats[p];
      }
    }
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

Colorset Mode_getColorset(const Mode *self, LedPos pos)
{
  return Mode_getColorsetMut((Mode *)self, pos);
}

Colorset Mode_getColorsetMut(Mode *self, LedPos pos)
{
  Pattern *pat = Mode_getPattern(self, pos);
  if (!pat) {
    Colorset empty;
    Colorset_init(&empty);
    return empty;
  }
  return Pattern_getColorset(pat);
}

PatternID Mode_getPatternID(const Mode *self, LedPos pos)
{
  const Pattern *pat = Mode_getPatternConst(self, pos);
  if (!pat) {
    return PATTERN_NONE;
  }
  return Pattern_getPatternID(pat);
}

bool Mode_setPattern(Mode *self, PatternID pat, LedPos pos, const PatternArgs *args, const Colorset *set)
{
  Colorset newSet = set ? *set : Mode_getColorsetMut(self, ((pos < LED_COUNT) && self->m_singlePats[pos]) ? pos : LED_ANY);
  switch (pos) {
  case LED_ANY:
  case LED_ALL:
#if VORTEX_SLIM == 0
  case LED_MULTI:
    if (self->m_multiPat) {
      Pattern_destroy(self->m_multiPat);
      vfree(self->m_multiPat);
      self->m_multiPat = NULL;
    }
    if (isMultiLedPatternID(pat)) {
      self->m_multiPat = PatternBuilder_makeMulti(pat, args);
      if (self->m_multiPat) {
        Pattern_setColorset(self->m_multiPat, &newSet);
      }
      Mode_clearPattern(self, LED_ALL_SINGLE);
      return true;
    }
    if (pos == LED_MULTI) {
      return false;
    }
#endif
  case LED_ALL_SINGLE:
#if FIXED_LED_COUNT == 0
    if (!self->m_singlePats) {
      return false;
    }
#endif
    for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
      if (!Mode_setPattern(self, pat, p, args, &newSet)) {
        return false;
      }
    }
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
      vfree(self->m_singlePats[pos]);
    }
    self->m_singlePats[pos] = PatternBuilder_makeSingle(pat, args);
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
    vfree(self->m_singlePats[to]);
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

bool Mode_setColorset(Mode *self, const Colorset *set, LedPos pos)
{
  switch (pos) {
  case LED_ANY:
  case LED_ALL:
#if VORTEX_SLIM == 0
  case LED_MULTI:
    if (self->m_multiPat) {
      Pattern_setColorset(self->m_multiPat, set);
    }
    if (pos == LED_MULTI) {
      return true;
    }
#endif
  case LED_ALL_SINGLE:
#if FIXED_LED_COUNT == 0
    if (!self->m_singlePats) {
      return false;
    }
#endif
    for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
      if (self->m_singlePats[p]) {
        Pattern_setColorset(self->m_singlePats[p], set);
      }
    }
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
  Colorset_init(&empty);
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
    Pattern *pat = Mode_getPattern(self, pos);
    if (!pat) {
      continue;
    }
    Pattern_setArg(pat, param, value);
  }
}

uint8_t Mode_getArg(Mode *self, uint8_t index, LedPos pos)
{
  Pattern *pat = Mode_getPattern(self, pos);
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

bool Mode_hasMultiLed(const Mode *self)
{
#if VORTEX_SLIM == 0
  return (self->m_multiPat != NULL);
#else
  return false;
#endif
}

bool Mode_hasSingleLed(const Mode *self)
{
  for (LedPos i = LED_FIRST; i < MODE_LEDCOUNT; ++i) {
    if (self->m_singlePats[i]) {
      return true;
    }
  }
  return false;
}

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
  return (firstPat != NULL);
}

bool Mode_hasSparseSingleLed(const Mode *self)
{
  switch (Mode_getSingleLedMap(self)) {
  case MAP_LED_ALL:
  case 0:
    return false;
  default:
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
    if (self->m_singlePats[i] && Pattern_getPatternID(self->m_singlePats[i]) != PATTERN_NONE) {
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

void Mode_test(void)
{
  INFO_LOG("== Beginning Mode Tests ==\n");

  INFO_LOG("= Testing Mode::setPattern =\n");

  Mode modeTest;
  Mode_init(&modeTest);

  assert(Mode_getPatternID(&modeTest, LED_MULTI) == PATTERN_NONE);
  assert(Mode_getColorsetConst(&modeTest, LED_MULTI).numColors == 0);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Mode_getPatternIDAt(&modeTest, pos) == PATTERN_NONE);
    assert(Mode_getColorsetAtConst(&modeTest, pos).numColors == 0);
    assert(Mode_getPatternAt(&modeTest, pos) == NULL);
  }

  INFO_LOG("Test Mode default constructor passed.\n");

  PatternID testPatternID = PATTERN_SOLID;
  PatternArgs testPatternArgs;
  PatternArgs_init4(&testPatternArgs, 100, 255, 0, 10);
  Colorset testColorset;
  Colorset_initColors(&testColorset, (RGBColor){0, 0, 255});

  Mode_setPattern(&modeTest, testPatternID, LED_ANY, &testPatternArgs, &testColorset);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Mode_getPatternIDAt(&modeTest, pos) == testPatternID);
    assert(Mode_getColorsetAt(&modeTest, pos)->equals(&testColorset));
  }
  INFO_LOG("Test setPattern passed.\n");

  Mode modeMultiLedTest;
  Mode_init(&modeMultiLedTest);
  PatternID multiPatternID = PATTERN_MULTI_FIRST;
  PatternArgs multiPatternArgs;
  PatternArgs_init4(&multiPatternArgs, 200, 200, 200, 5);
  Colorset multiColorset;
  Colorset_initColors(&multiColorset, (RGBColor){255, 0, 0});

  Mode_setPattern(&modeMultiLedTest, multiPatternID, LED_ANY, &multiPatternArgs, &multiColorset);

  assert(Mode_getPatternID(&modeMultiLedTest, LED_ANY) == multiPatternID);
  assert(Mode_getColorsetConst(&modeMultiLedTest, LED_ANY).equals(&multiColorset));
  INFO_LOG("Test setPattern with multi-led pattern passed.\n");

  Mode modeBothTest;
  Mode_init(&modeBothTest);
  PatternID singlePatternID = PATTERN_SOLID;
  PatternArgs singlePatternArgs;
  PatternArgs_init4(&singlePatternArgs, 100, 255, 0, 10);
  Colorset singleColorset;
  Colorset_initColors(&singleColorset, (RGBColor){0, 0, 255});

  Mode_setPattern(&modeBothTest, singlePatternID, LED_ANY, &singlePatternArgs, &singleColorset);
  Mode_setPattern(&modeBothTest, multiPatternID, LED_ANY, &multiPatternArgs, &multiColorset);

  assert(Mode_hasMultiLed(&modeBothTest) && Mode_hasSingleLed(&modeBothTest));
  INFO_LOG("Test setPattern with single-led and multi-led patterns simultaneously passed.\n");

  Mode modeSetPatternAtTest;
  Mode_init(&modeSetPatternAtTest);
  LedPos setPatternAtPosition = LED_5;
  Mode_setPatternAt(&modeSetPatternAtTest, setPatternAtPosition, testPatternID, &testPatternArgs, &testColorset);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == setPatternAtPosition) {
      assert(Mode_getPatternIDAt(&modeSetPatternAtTest, pos) == testPatternID);
      assert(Mode_getColorsetAt(&modeSetPatternAtTest, pos)->equals(&testColorset));
    } else {
      assert(Mode_getPatternIDAt(&modeSetPatternAtTest, pos) == PATTERN_NONE);
    }
  }
  INFO_LOG("Test setPatternAt passed.\n");

  Colorset newColorset;
  Colorset_initColors(&newColorset, (RGBColor){255, 255, 0}, (RGBColor){255, 165, 0}, (RGBColor){0, 255, 0});
  LedPos setColorsetPosition = LED_3;
  Mode_setColorsetAt(&modeTest, setColorsetPosition, &newColorset);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == setColorsetPosition) {
      assert(Mode_getColorsetAt(&modeTest, pos)->equals(&newColorset));
    } else {
      assert(Mode_getColorsetAt(&modeTest, pos)->equals(&testColorset));
    }
  }
  INFO_LOG("Test setColorsetAt passed.\n");

  Mode modeClearSingleLedTest;
  Mode_init(&modeClearSingleLedTest);
  Mode_setPattern(&modeClearSingleLedTest, testPatternID, LED_ANY, &testPatternArgs, &testColorset);
  Mode_clearPatterns(&modeClearSingleLedTest);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Mode_getPatternIDAt(&modeClearSingleLedTest, pos) == PATTERN_NONE);
  }
  INFO_LOG("Test clearPattern with single-led pattern passed.\n");

  Mode modeClearMultiLedTest;
  Mode_init(&modeClearMultiLedTest);
  Mode_setPattern(&modeClearMultiLedTest, multiPatternID, LED_ANY, &multiPatternArgs, &multiColorset);
  Mode_clearPatterns(&modeClearMultiLedTest);

  assert(Mode_getPatternID(&modeClearMultiLedTest, LED_ANY) == PATTERN_NONE);
  INFO_LOG("Test clearPattern with multi-led pattern passed.\n");

  Mode_clearPatterns(&modeBothTest);

  assert(Mode_getPatternID(&modeBothTest, LED_ANY) == PATTERN_NONE);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Mode_getPatternIDAt(&modeBothTest, pos) == PATTERN_NONE);
  }
  INFO_LOG("Test clearPattern with both single-led and multi-led patterns passed.\n");

  Mode modeClearColorsetTest;
  Mode_init(&modeClearColorsetTest);
  Mode_setPattern(&modeClearColorsetTest, testPatternID, LED_ANY, &testPatternArgs, &testColorset);
  Mode_clearColorsets(&modeClearColorsetTest);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Colorset_numColors(Mode_getColorsetAt(&modeClearColorsetTest, pos)) == 0);
  }
  INFO_LOG("Test clearColorset passed.\n");

  Mode modeClearColorsetAtTest;
  Mode_init(&modeClearColorsetAtTest);
  Mode_setPattern(&modeClearColorsetAtTest, testPatternID, LED_ANY, &testPatternArgs, &testColorset);
  LedPos clearColorsetAtPosition = LED_3;
  Mode_clearColorsetAt(&modeClearColorsetAtTest, clearColorsetAtPosition);

  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    if (pos == clearColorsetAtPosition) {
      assert(Colorset_numColors(Mode_getColorsetAt(&modeClearColorsetAtTest, pos)) == 0);
    } else {
      assert(Colorset_numColors(Mode_getColorsetAt(&modeClearColorsetAtTest, pos)) == Colorset_numColors(&testColorset));
    }
  }
  INFO_LOG("Test clearColorsetAt passed.\n");

  Mode modeClearMultiColorsetsTest;
  Mode_init(&modeClearMultiColorsetsTest);
  Mode_setPattern(&modeClearMultiColorsetsTest, multiPatternID, LED_ANY, &multiPatternArgs, &multiColorset);
  Mode_clearColorsets(&modeClearMultiColorsetsTest);

  assert(Colorset_numColors(Mode_getColorsetConst(&modeClearMultiColorsetsTest, LED_ANY)) == 0);
  INFO_LOG("Test clearColorsets with multi-led pattern passed.\n");

  Mode_setPattern(&modeBothTest, PATTERN_BASIC, LED_ANY, NULL, NULL);
  Mode_clearColorsets(&modeBothTest);

  assert(Colorset_numColors(Mode_getColorsetConst(&modeBothTest, LED_ANY)) == 0);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Colorset_numColors(Mode_getColorsetAt(&modeBothTest, pos)) == 0);
  }
  INFO_LOG("Test clearColorsets with both single-led and multi-led patterns passed.\n");

  Mode modeSparseTest;
  Mode_init(&modeSparseTest);
  Mode_setPatternAt(&modeSparseTest, LED_3, testPatternID, &testPatternArgs, &testColorset);
  Mode_setPatternAt(&modeSparseTest, LED_5, testPatternID, &testPatternArgs, &testColorset);
  Mode_setPatternAt(&modeSparseTest, LED_7, testPatternID, &testPatternArgs, &testColorset);

  assert(Mode_hasSparseSingleLed(&modeSparseTest));
  INFO_LOG("Test hasSparseSingleLed with sparse single-led patterns passed.\n");

  Mode modeNoSingleLedTest;
  Mode_init(&modeNoSingleLedTest);
  assert(Mode_getSingleLedMap(&modeNoSingleLedTest) == 0);
  INFO_LOG("Test getSingleLedMap with no single-led patterns passed.\n");

  Mode modeAllSingleLedTest;
  Mode_init(&modeAllSingleLedTest);
  Mode_setPattern(&modeAllSingleLedTest, testPatternID, LED_ANY, &testPatternArgs, &testColorset);
  assert(Mode_getSingleLedMap(&modeAllSingleLedTest) == MAP_LED_ALL);
  INFO_LOG("Test getSingleLedMap with all single-led patterns passed.\n");

  assert(Mode_getSingleLedMap(&modeSparseTest) == ((LedMap)1 << LED_3 | (LedMap)1 << LED_5 | (LedMap)1 << LED_7));
  INFO_LOG("Test getSingleLedMap with sparse single-led patterns passed.\n");

  Mode modeOnlyMultiTest;
  Mode_init(&modeOnlyMultiTest);
  Mode_setPattern(&modeOnlyMultiTest, multiPatternID, LED_ANY, &multiPatternArgs, &multiColorset);
  assert(Mode_isMultiLed(&modeOnlyMultiTest));
  INFO_LOG("Test isMultiLed with only multi-led pattern passed.\n");

  assert(!Mode_isMultiLed(&modeBothTest));
  INFO_LOG("Test isMultiLed with both single-led and multi-led patterns passed.\n");

  Mode modeSetClearAtTest;
  Mode_init(&modeSetClearAtTest);
  Mode_setPatternAt(&modeSetClearAtTest, LED_5, testPatternID, &testPatternArgs, &testColorset);
  Mode_clearPatternAt(&modeSetClearAtTest, LED_5);

  assert(Mode_getPatternIDAt(&modeSetClearAtTest, LED_5) == PATTERN_NONE);
  assert(Mode_getColorsetAt(&modeSetClearAtTest, LED_5) == NULL);
  INFO_LOG("Test setPatternAt and clearPatternAt for specific position passed.\n");

  Mode modeSetClearColorsetAtTest;
  Mode_init(&modeSetClearColorsetAtTest);
  Mode_setPatternAt(&modeSetClearColorsetAtTest, LED_5, testPatternID, &testPatternArgs, &testColorset);
  Mode_clearColorsetAt(&modeSetClearColorsetAtTest, LED_5);

  assert(Colorset_numColors(Mode_getColorsetAt(&modeSetClearColorsetAtTest, LED_5)) == 0);
  INFO_LOG("Test setColorsetAt and clearColorsetAt for specific position passed.\n");

  Mode modeMultiClearTest;
  Mode_init(&modeMultiClearTest);
  Mode_setPattern(&modeMultiClearTest, multiPatternID, LED_ANY, &multiPatternArgs, &multiColorset);
  Mode_clearMultiColorset(&modeMultiClearTest);

  assert(Mode_getPatternID(&modeMultiClearTest, LED_ANY) == multiPatternID);
  assert(Colorset_numColors(Mode_getColorsetConst(&modeMultiClearTest, LED_ANY)) == 0);
  INFO_LOG("Test clearMultiColorset with multi-led pattern passed.\n");

  PatternID invalidID = (PatternID)(PATTERN_COUNT + 1);

  Mode modeInvalidPatternIDTest;
  Mode_init(&modeInvalidPatternIDTest);
  bool success = Mode_setPattern(&modeInvalidPatternIDTest, invalidID, LED_ANY, &testPatternArgs, &testColorset);
  assert(!success);
  INFO_LOG("Test setPattern with invalid pattern ID passed.\n");

  Mode modeInvalidPatternIDAtTest;
  Mode_init(&modeInvalidPatternIDAtTest);
  success = Mode_setPatternAt(&modeInvalidPatternIDAtTest, LED_3, invalidID, &testPatternArgs, &testColorset);
  assert(!success);
  INFO_LOG("Test setPatternAt with invalid pattern ID passed.\n");

  Mode modeInvalidMultiPatternIDTest;
  Mode_init(&modeInvalidMultiPatternIDTest);
  success = Mode_setMultiPat(&modeInvalidMultiPatternIDTest, PATTERN_SINGLE_LAST, &testPatternArgs, &testColorset);
  assert(!success);
  INFO_LOG("Test setMultiPat with invalid pattern ID passed.\n");

  LedPos invalidPos = (LedPos)(LED_COUNT + 1);

  Mode modeInvalidLEDPositionTest;
  Mode_init(&modeInvalidLEDPositionTest);
  success = Mode_setPatternAt(&modeInvalidLEDPositionTest, invalidPos, testPatternID, &testPatternArgs, &testColorset);
  assert(success);
  INFO_LOG("Test setPattern with invalid LED position passed.\n");

  Mode modeInvalidLEDPositionSetColorsetTest;
  Mode_init(&modeInvalidLEDPositionSetColorsetTest);
  success = Mode_setColorsetAt(&modeInvalidLEDPositionSetColorsetTest, invalidPos, &testColorset);
  assert(success);
  INFO_LOG("Test setColorsetAt with invalid LED position passed.\n");

  Mode modeInvalidLEDPositionClearPatternTest;
  Mode_init(&modeInvalidLEDPositionClearPatternTest);
  Mode_setPattern(&modeInvalidLEDPositionClearPatternTest, PATTERN_BLENDSTROBE, LED_ANY, NULL, NULL);
  Mode_clearPatternAt(&modeInvalidLEDPositionClearPatternTest, invalidPos);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Mode_getPatternAt(&modeInvalidLEDPositionClearPatternTest, pos) != NULL);
  }
  INFO_LOG("Test clearPatternAt with invalid LED position passed.\n");

  Mode modeInvalidLEDPositionClearColorsetTest;
  Mode_init(&modeInvalidLEDPositionClearColorsetTest);
  Mode_setPattern(&modeInvalidLEDPositionClearColorsetTest, PATTERN_BLEND, LED_ANY, NULL, NULL);
  Mode_clearColorsetAt(&modeInvalidLEDPositionClearColorsetTest, invalidPos);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Mode_getColorsetAt(&modeInvalidLEDPositionClearColorsetTest, pos) != NULL);
  }
  INFO_LOG("Test clearColorsetAt with invalid LED position passed.\n");

  Mode modePatternNoneTest;
  Mode_init(&modePatternNoneTest);
  Mode_setPatternAt(&modePatternNoneTest, LED_2, PATTERN_NONE, &testPatternArgs, &testColorset);
  assert(Mode_getPatternIDAt(&modePatternNoneTest, LED_2) == PATTERN_NONE);
  INFO_LOG("Test setPatternAt with PATTERN_NONE passed.\n");

  Mode modeSetPatternNoneTest;
  Mode_init(&modeSetPatternNoneTest);
  Mode_setPattern(&modeSetPatternNoneTest, PATTERN_NONE, LED_ANY, &testPatternArgs, &testColorset);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Mode_getPatternIDAt(&modeSetPatternNoneTest, pos) == PATTERN_NONE);
  }
  INFO_LOG("Test setPattern with PATTERN_NONE passed.\n");

  Mode modeSetPatternMultiLastTest;
  Mode_init(&modeSetPatternMultiLastTest);
  Mode_setPattern(&modeSetPatternMultiLastTest, PATTERN_MULTI_LAST, LED_ANY, &testPatternArgs, &testColorset);
  assert(Mode_getPatternID(&modeSetPatternMultiLastTest, LED_ANY) == PATTERN_MULTI_LAST);
  assert(Mode_getMultiPatID(&modeSetPatternMultiLastTest) == PATTERN_MULTI_LAST);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    assert(Mode_getPatternIDAt(&modeSetPatternNoneTest, pos) == PATTERN_NONE);
  }
  INFO_LOG("Test setPattern with PATTERN_MULTI_LAST passed.\n");

  Mode modeSetMultiPatMultiFirstTest;
  Mode_init(&modeSetMultiPatMultiFirstTest);
  Mode_setMultiPat(&modeSetMultiPatMultiFirstTest, PATTERN_MULTI_FIRST, &testPatternArgs, &testColorset);
  assert(Mode_getPatternID(&modeSetMultiPatMultiFirstTest, LED_ANY) == PATTERN_MULTI_FIRST);
  INFO_LOG("Test setMultiPat with PATTERN_MULTI_FIRST passed.\n");

  Mode modeSetMultiPatMultiLastTest;
  Mode_init(&modeSetMultiPatMultiLastTest);
  Mode_setMultiPat(&modeSetMultiPatMultiLastTest, PATTERN_MULTI_LAST, &testPatternArgs, &testColorset);
  assert(Mode_getPatternID(&modeSetMultiPatMultiLastTest, LED_ANY) == PATTERN_MULTI_LAST);
  INFO_LOG("Test setMultiPat with PATTERN_MULTI_LAST passed.\n");

  Mode modeSetMultiPatSingleFirstTest;
  Mode_init(&modeSetMultiPatSingleFirstTest);
  bool setMultiPatSingleFirstSuccess = Mode_setMultiPat(&modeSetMultiPatSingleFirstTest, PATTERN_SINGLE_FIRST, &testPatternArgs, &testColorset);
  assert(!setMultiPatSingleFirstSuccess);
  INFO_LOG("Test setMultiPat with PATTERN_SINGLE_FIRST passed.\n");

  Mode modeSetMultiPatSingleLastTest;
  Mode_init(&modeSetMultiPatSingleLastTest);
  bool setMultiPatSingleLastSuccess = Mode_setMultiPat(&modeSetMultiPatSingleLastTest, PATTERN_SINGLE_LAST, &testPatternArgs, &testColorset);
  assert(!setMultiPatSingleLastSuccess);
  INFO_LOG("Test setMultiPat with PATTERN_SINGLE_LAST passed.\n");
}

#endif
