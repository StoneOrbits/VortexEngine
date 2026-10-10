#include "PatternBuilder.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"
#include "../Memory/Memory.h"

#include "Multi/Sequencer/SequencedPattern.h"
#include "Multi/Sequencer/ChaserPattern.h"
#include "Multi/Sequencer/Sequence.h"

#include "Multi/TheaterChasePattern.h"
#include "Multi/HueShiftPattern.h"
#include "Multi/ZigzagPattern.h"
#include "Multi/DripPattern.h"
#include "Multi/DripMorphPattern.h"
#include "Multi/CrossDopsPattern.h"
#include "Multi/DoubleStrobePattern.h"
#include "Multi/MeteorPattern.h"
#include "Multi/SparkleTracePattern.h"
#include "Multi/VortexWipePattern.h"
#include "Multi/WarpPattern.h"
#include "Multi/WarpWormPattern.h"
#include "Multi/FillPattern.h"
#include "Multi/SnowballPattern.h"
#include "Multi/LighthousePattern.h"
#include "Multi/PulsishPattern.h"
#include "Multi/BouncePattern.h"
#include "Multi/BackStrobePattern.h"
#include "Multi/VortexPattern.h"

#include "Single/SingleLedPattern.h"
#include "Single/BasicPattern.h"
#include "Single/BlendPattern.h"
#include "Single/SolidPattern.h"

// every pattern type that can be instantiated, the pattern pool blocks
// are sized to fit the largest of them
typedef union AnyPattern {
  BlinkPattern blink;
  SolidPattern solid;
#if VORTEX_SLIM == 0
  HueShiftPattern hueShift;
  TheaterChasePattern theaterChase;
  ChaserPattern chaser;
  ZigzagPattern zigzag;
  DripPattern drip;
  DripMorphPattern dripMorph;
  CrossDopsPattern crossDops;
  DoubleStrobePattern doubleStrobe;
  MeteorPattern meteor;
  SparkleTracePattern sparkleTrace;
  VortexWipePattern vortexWipe;
  WarpPattern warp;
  WarpWormPattern warpWorm;
  SnowballPattern snowball;
  LighthousePattern lighthouse;
  PulsishPattern pulsish;
  BouncePattern bounce;
  BackStrobePattern backStrobe;
  VortexPattern vortex;
#endif
} AnyPattern;

// the pool all patterns are allocated from
static MemPool s_patternPool;
static MEMPOOL_STORAGE(s_patternStorage, sizeof(AnyPattern), PATTERN_POOL_SIZE);

bool PatternBuilder_init()
{
  MemPool_init(&s_patternPool, s_patternStorage, sizeof(AnyPattern), PATTERN_POOL_SIZE);
  return true;
}

void *PatternBuilder_allocPattern()
{
  return MemPool_alloc(&s_patternPool);
}

void PatternBuilder_freePattern(Pattern *pat)
{
  MemPool_free(&s_patternPool, pat);
}

// allocate + construct a multi led pattern with the given args
#define MAKE_MULTI_PATTERN(CLASSNAME) \
  do { \
    CLASSNAME *p = (CLASSNAME *)PatternBuilder_allocPattern(); \
    if (p) { \
      CLASSNAME##_init(p, &args); \
      return (Pattern *)p; \
    } \
    return NULL; \
  } while (0)

Pattern *PatternBuilder_make(PatternID id, const PatternArgs *args)
{
#if VORTEX_SLIM == 0
  if (id > PATTERN_LAST) {
#else
  if (id > PATTERN_SINGLE_LAST) {
#endif
    DEBUG_LOGF("Invalid pattern id: %u", id);
    id = PATTERN_FIRST;
  }
#if VORTEX_SLIM == 0
  if (isMultiLedPatternID(id)) {
    return (Pattern *)PatternBuilder_makeMulti(id, args);
  }
#endif
  return PatternBuilder_makeSingle(id, args);
}

Pattern *PatternBuilder_dupe(const Pattern *pat)
{
  if (!pat) {
    return NULL;
  }
  PatternArgs args;
  Pattern_getArgs(pat, &args);
  Pattern *newPat = PatternBuilder_make(pat->m_patternID, &args);
  if (!newPat) {
    return NULL;
  }
  Colorset cs = Pattern_getColorset(pat);
  Pattern_setColorset(newPat, &cs);
  newPat->vtable->bind(newPat, pat->m_ledPos);
  return newPat;
}

// generate a single LED pattern (NULL if patternid is not single LED)
SingleLedPattern *PatternBuilder_makeSingle(PatternID id, const PatternArgs *args)
{
  if (!isSingleLedPatternID(id)) {
    return NULL;
  }
  Pattern *pat = PatternBuilder_makeInternal(id, args);
  if (!pat) {
    return NULL;
  }
  // don't set any flags on single pattersn
  return (SingleLedPattern *)pat;
}

// generate a multi LED pattern (NULL if patternid is not multi LED)
MultiLedPattern *PatternBuilder_makeMulti(PatternID id, const PatternArgs *args)
{
  if (!isMultiLedPatternID(id)) {
    return NULL;
  }
  Pattern *pat = PatternBuilder_makeInternal(id, args);
  if (!pat) {
    return NULL;
  }
  return (MultiLedPattern *)pat;
}

Pattern *PatternBuilder_unserialize(ByteStream *buffer)
{
  PatternID id = PATTERN_NONE;
  if (!ByteStream_unserialize8(buffer, (uint8_t *)&id)) {
    return NULL;
  }
  Pattern *pat = PatternBuilder_make(id, NULL);
  if (!pat) {
    return NULL;
  }
  if (!Pattern_unserialize(pat, buffer)) {
    Pattern_destroy(pat);
    PatternBuilder_freePattern(pat);
    return NULL;
  }
  return pat;
}

Pattern *PatternBuilder_makeInternal(PatternID id, const PatternArgs *args)
{
  if (id == PATTERN_NONE) {
    // no error
    return NULL;
  }
  Pattern *pat = PatternBuilder_generate(id, args);
  if (!pat) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
  // set the pattern ID on the instance
  Pattern_setPatternID(pat, id);
  return pat;
}

// This is just the default arguments for any given pattern id
// it will *not* indicate the true amount of arguments a pattern has
// Some pattern ids are purely defined by their unique arguments
// to a class, so calling them 'default' might be misleading
void PatternBuilder_getDefaultArgs(PatternID id, PatternArgs *out)
{
  switch (id) {
    // =====================
    //  Single Led Patterns:
    case PATTERN_STROBE: PatternArgs_make(out, STROBE_ON_DURATION, STROBE_OFF_DURATION); return;
    case PATTERN_HYPERSTROBE: PatternArgs_make(out, HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION); return;
    case PATTERN_PICOSTROBE: PatternArgs_make(out, PICOSTROBE_ON_DURATION, PICOSTROBE_OFF_DURATION); return;
    case PATTERN_STROBIE: PatternArgs_make(out, STROBIE_ON_DURATION, STROBIE_OFF_DURATION); return;
    case PATTERN_DOPS: PatternArgs_make(out, DOPS_ON_DURATION, DOPS_OFF_DURATION); return;
    case PATTERN_ULTRADOPS: PatternArgs_make(out, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION); return;
    case PATTERN_STROBEGAP: PatternArgs_make(out, STROBE_ON_DURATION, STROBE_OFF_DURATION, 25); return;
    case PATTERN_HYPERGAP: PatternArgs_make(out, HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION, 218); return;
    case PATTERN_PICOGAP: PatternArgs_make(out, PICOSTROBE_ON_DURATION, PICOSTROBE_OFF_DURATION, 90); return;
    case PATTERN_STROBIEGAP: PatternArgs_make(out, STROBIE_ON_DURATION, STROBIE_OFF_DURATION, 100); return;
    case PATTERN_DOPSGAP: PatternArgs_make(out, DOPS_ON_DURATION, DOPS_OFF_DURATION, 40); return;
    case PATTERN_ULTRAGAP: PatternArgs_make(out, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, 55); return;
    case PATTERN_BLINKIE: PatternArgs_make(out, 3, 1, 65); return;
    case PATTERN_GHOSTCRUSH: PatternArgs_make(out, 3, 1, 18); return;
    case PATTERN_DOUBLEDOPS: PatternArgs_make(out, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, DOPS_OFF_DURATION, 0, 2); return;
    case PATTERN_CHOPPER: PatternArgs_make(out, ULTRADOPS_ON_DURATION, STROBIE_OFF_DURATION, 0, STROBE_ON_DURATION); return;
    case PATTERN_DASHGAP: PatternArgs_make(out, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, 20, 20); return;
    case PATTERN_DASHDOPS: PatternArgs_make(out, DOPS_ON_DURATION, DOPS_OFF_DURATION, DOPS_OFF_DURATION, 18); return;
    case PATTERN_DASHCRUSH: PatternArgs_make(out, 4, 1, 10, 18); return;
    case PATTERN_ULTRADASH: PatternArgs_make(out, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, ULTRADOPS_OFF_DURATION, 14); return;
    case PATTERN_GAPCYCLE: PatternArgs_make(out, STROBIE_ON_DURATION, STROBE_OFF_DURATION, 12, 25, 2); return;
    case PATTERN_DASHCYCLE: PatternArgs_make(out, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, ULTRADOPS_OFF_DURATION, 30, 2); return;
    case PATTERN_TRACER: PatternArgs_make(out, 3, 0, 0, 20, 1); return;
    case PATTERN_RIBBON: PatternArgs_make(out, RIBBON_DURATION); return;
    case PATTERN_MINIRIBBON: PatternArgs_make(out, 1); return;
    case PATTERN_BLEND: PatternArgs_make(out, BLEND_ON_DURATION, BLEND_OFF_DURATION, 0, 0, 0, 5, 0); return;
    case PATTERN_BLENDSTROBE: PatternArgs_make(out, STROBE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 10, 0); return;
    case PATTERN_BLENDSTROBEGAP: PatternArgs_make(out, STROBE_ON_DURATION, STROBE_OFF_DURATION, 25, 0, 0, 10, 0); return;
    case PATTERN_COMPLEMENTARY_BLEND: PatternArgs_make(out, BLEND_ON_DURATION, BLEND_OFF_DURATION, 0, 0, 0, 5, 1); return;
    case PATTERN_COMPLEMENTARY_BLENDSTROBE: PatternArgs_make(out, STROBE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 10, 1); return;
    case PATTERN_COMPLEMENTARY_BLENDSTROBEGAP: PatternArgs_make(out, STROBE_ON_DURATION, STROBE_OFF_DURATION, 25, 0, 0, 10, 1); return;
    case PATTERN_SOLID: PatternArgs_make(out, 250); return;

    // =====================
    //  Multi Led Patterns:
#if VORTEX_SLIM == 0
    case PATTERN_HUE_SCROLL: PatternArgs_make(out, 1, 1, 10); return;
    case PATTERN_THEATER_CHASE: PatternArgs_make(out, DOPS_ON_DURATION, DOPS_OFF_DURATION, 28); return;
    case PATTERN_CHASER: PatternArgs_make0(out); return;
    case PATTERN_ZIGZAG: PatternArgs_make(out, DOPS_ON_DURATION, 3, 3, 5, 55); return;
    case PATTERN_ZIPFADE: PatternArgs_make(out, DOPS_ON_DURATION, 2, 75, 9, 230); return;
    case PATTERN_DRIP: PatternArgs_make(out, STROBE_ON_DURATION, STROBE_OFF_DURATION, 150); return;
    case PATTERN_DRIPMORPH: PatternArgs_make(out, STROBE_ON_DURATION, STROBE_OFF_DURATION, 1); return;
    case PATTERN_CROSSDOPS: PatternArgs_make(out, DOPS_ON_DURATION, 2, 25); return;
    case PATTERN_DOUBLESTROBE: PatternArgs_make(out, DOPS_ON_DURATION, DOPS_OFF_DURATION, 115); return;
    case PATTERN_METEOR: PatternArgs_make(out, 1, 1, 20, 130); return;
    case PATTERN_SPARKLETRACE: PatternArgs_make(out, 1, 5, 3); return;
    case PATTERN_VORTEXWIPE: PatternArgs_make(out, DOPS_ON_DURATION, 3, 80); return;
    case PATTERN_WARP: PatternArgs_make(out, 3, DOPS_OFF_DURATION, 50); return;
    case PATTERN_WARPWORM: PatternArgs_make(out, DOPS_ON_DURATION, 10, 100); return;
    case PATTERN_SNOWBALL: PatternArgs_make(out, 3, 3, 33); return;
    case PATTERN_LIGHTHOUSE: PatternArgs_make(out, DOPS_ON_DURATION, 5, 22, 3, 3); return;
    case PATTERN_PULSISH: PatternArgs_make(out, DOPS_ON_DURATION, 6, 5, 1, 100); return;
    case PATTERN_FILL: PatternArgs_make(out, DOPS_ON_DURATION, 5, 50); return;
    case PATTERN_BOUNCE: PatternArgs_make(out, 10, 5, 50, 10); return;
    case PATTERN_SPLITSTROBIE: PatternArgs_make(out, DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, 16, 3, 10, PATTERN_DOPS, PATTERN_STROBIE); return;
    case PATTERN_BACKSTROBE: PatternArgs_make(out, DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION, 10, PATTERN_DOPS, PATTERN_HYPERSTROBE); return;
    case PATTERN_VORTEX: PatternArgs_make(out, 1, 1, 130); return;
    case PATTERN_NONE: break;
    default: break;
#else
    // in vortex slim just use DOPS for all mult-led
    case PATTERN_NONE: break;
    default: PatternArgs_make(out, DOPS_ON_DURATION, DOPS_OFF_DURATION); return;
#endif
  }
  PatternArgs_make0(out);
}

uint8_t PatternBuilder_numDefaultArgs(PatternID id)
{
  Pattern *pat = PatternBuilder_make(id, NULL);
  if (!pat) {
    return 0;
  }
  uint8_t numArgs = Pattern_getNumArgs(pat);
  Pattern_destroy(pat);
  PatternBuilder_freePattern(pat);
  return numArgs;
}

// This is responsible for instantiating the basic core patterns
Pattern *PatternBuilder_generate(PatternID id, const PatternArgs *userArgs)
{
  PatternArgs args;
  if (userArgs) {
    args = *userArgs;
  } else {
    PatternBuilder_getDefaultArgs(id, &args);
  }
  switch (id) {
    // =====================
    //  Single Led Patterns:
    default:
    case PATTERN_STROBE:
    case PATTERN_HYPERSTROBE:
    case PATTERN_PICOSTROBE:
    case PATTERN_STROBIE:
    case PATTERN_DOPS:
    case PATTERN_ULTRADOPS:
    case PATTERN_STROBEGAP:
    case PATTERN_HYPERGAP:
    case PATTERN_PICOGAP:
    case PATTERN_STROBIEGAP:
    case PATTERN_DOPSGAP:
    case PATTERN_ULTRAGAP:
    case PATTERN_BLINKIE:
    case PATTERN_GHOSTCRUSH:
    case PATTERN_DOUBLEDOPS:
    case PATTERN_CHOPPER:
    case PATTERN_DASHGAP:
    case PATTERN_DASHDOPS:
    case PATTERN_DASHCRUSH:
    case PATTERN_ULTRADASH:
    case PATTERN_GAPCYCLE:
    case PATTERN_DASHCYCLE:
    case PATTERN_TRACER:
    case PATTERN_RIBBON:
    // blink, dash, gap and blend patterns are all served by the single
    // merged BlinkPattern, the id decides which behaviour and which args
    case PATTERN_MINIRIBBON:
    case PATTERN_BLEND:
    case PATTERN_BLENDSTROBE:
    case PATTERN_BLENDSTROBEGAP:
    case PATTERN_COMPLEMENTARY_BLEND:
    case PATTERN_COMPLEMENTARY_BLENDSTROBE:
    case PATTERN_COMPLEMENTARY_BLENDSTROBEGAP: return (Pattern *)BlinkPattern_make(id, &args);
    case PATTERN_SOLID: return (Pattern *)SolidPattern_make(id, &args);

    // =====================
    //  Multi Led Patterns:
#if VORTEX_SLIM == 0
    case PATTERN_HUE_SCROLL: MAKE_MULTI_PATTERN(HueShiftPattern);
    case PATTERN_THEATER_CHASE: MAKE_MULTI_PATTERN(TheaterChasePattern);
    case PATTERN_CHASER: MAKE_MULTI_PATTERN(ChaserPattern);
    case PATTERN_ZIGZAG:
    case PATTERN_ZIPFADE: MAKE_MULTI_PATTERN(ZigzagPattern);
    case PATTERN_DRIP: MAKE_MULTI_PATTERN(DripPattern);
    case PATTERN_DRIPMORPH: MAKE_MULTI_PATTERN(DripMorphPattern);
    case PATTERN_CROSSDOPS: MAKE_MULTI_PATTERN(CrossDopsPattern);
    case PATTERN_DOUBLESTROBE: MAKE_MULTI_PATTERN(DoubleStrobePattern);
    case PATTERN_METEOR: MAKE_MULTI_PATTERN(MeteorPattern);
    case PATTERN_SPARKLETRACE: MAKE_MULTI_PATTERN(SparkleTracePattern);
    case PATTERN_VORTEXWIPE: MAKE_MULTI_PATTERN(VortexWipePattern);
    case PATTERN_WARP: MAKE_MULTI_PATTERN(WarpPattern);
    case PATTERN_WARPWORM: MAKE_MULTI_PATTERN(WarpWormPattern);
    case PATTERN_SNOWBALL: MAKE_MULTI_PATTERN(SnowballPattern);
    case PATTERN_LIGHTHOUSE: MAKE_MULTI_PATTERN(LighthousePattern);
    case PATTERN_PULSISH: MAKE_MULTI_PATTERN(PulsishPattern);
    case PATTERN_FILL: MAKE_MULTI_PATTERN(FillPattern);
    case PATTERN_BOUNCE: MAKE_MULTI_PATTERN(BouncePattern);
    case PATTERN_SPLITSTROBIE:
    case PATTERN_BACKSTROBE: MAKE_MULTI_PATTERN(BackStrobePattern);
    case PATTERN_VORTEX: MAKE_MULTI_PATTERN(VortexPattern);
    case PATTERN_NONE: return NULL;
#else
    // in vortex slim just use basic pattern for all multi led
    case PATTERN_NONE: return NULL;
#endif
  }
  DEBUG_LOGF("Unknown pattern id: %u", id);
  return NULL;
}
