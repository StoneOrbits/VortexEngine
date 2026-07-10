#include "PatternBuilder.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"
#include "../Memory/Memory.h"

#include "Pattern.h"

#include "Single/SingleLedPattern.h"
#include "Single/BasicPattern.h"
#include "Single/BlendPattern.h"
#include "Single/SolidPattern.h"

#if VORTEX_SLIM == 0
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
#endif

// Forward declarations
static Pattern *PatternBuilder_generate(PatternID id, const PatternArgs *args);
static Pattern *PatternBuilder_makeInternal(PatternID id, const PatternArgs *args);

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
    return PatternBuilder_makeMulti(id, args);
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
  Pattern *newPat = PatternBuilder_make(Pattern_getPatternID(pat), &args);
  if (!newPat) {
    return NULL;
  }
  Colorset _cs = Pattern_getColorset(pat);
  Pattern_setColorset(newPat, &_cs);
  Pattern_bind(newPat, Pattern_getLedPos(pat));
  return newPat;
}

Pattern *PatternBuilder_makeSingle(PatternID id, const PatternArgs *args)
{
  if (!isSingleLedPatternID(id)) {
    return NULL;
  }
  return PatternBuilder_makeInternal(id, args);
}

Pattern *PatternBuilder_makeMulti(PatternID id, const PatternArgs *args)
{
  if (!isMultiLedPatternID(id)) {
    return NULL;
  }
  return PatternBuilder_makeInternal(id, args);
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
    vfree(pat);
    return NULL;
  }
  return pat;
}

Pattern *PatternBuilder_makeInternal(PatternID id, const PatternArgs *args)
{
  if (id == PATTERN_NONE) {
    return NULL;
  }
  Pattern *pat = PatternBuilder_generate(id, args);
  if (!pat) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
  pat->patternID = id;
  return pat;
}

PatternArgs PatternBuilder_getDefaultArgs(PatternID id)
{
  PatternArgs args;
  PatternArgs_init(&args);
  switch (id) {
    case PATTERN_STROBE: PatternArgs_init2(&args, STROBE_ON_DURATION, STROBE_OFF_DURATION); break;
    case PATTERN_HYPERSTROBE: PatternArgs_init2(&args, HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION); break;
    case PATTERN_PICOSTROBE: PatternArgs_init2(&args, PICOSTROBE_ON_DURATION, PICOSTROBE_OFF_DURATION); break;
    case PATTERN_STROBIE: PatternArgs_init2(&args, STROBIE_ON_DURATION, STROBIE_OFF_DURATION); break;
    case PATTERN_DOPS: PatternArgs_init2(&args, DOPS_ON_DURATION, DOPS_OFF_DURATION); break;
    case PATTERN_ULTRADOPS: PatternArgs_init2(&args, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION); break;
    case PATTERN_STROBEGAP: PatternArgs_init3(&args, STROBE_ON_DURATION, STROBE_OFF_DURATION, 25); break;
    case PATTERN_HYPERGAP: PatternArgs_init3(&args, HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION, 218); break;
    case PATTERN_PICOGAP: PatternArgs_init3(&args, PICOSTROBE_ON_DURATION, PICOSTROBE_OFF_DURATION, 90); break;
    case PATTERN_STROBIEGAP: PatternArgs_init3(&args, STROBIE_ON_DURATION, STROBIE_OFF_DURATION, 100); break;
    case PATTERN_DOPSGAP: PatternArgs_init3(&args, DOPS_ON_DURATION, DOPS_OFF_DURATION, 40); break;
    case PATTERN_ULTRAGAP: PatternArgs_init3(&args, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, 55); break;
    case PATTERN_BLINKIE: PatternArgs_init3(&args, 3, 1, 65); break;
    case PATTERN_GHOSTCRUSH: PatternArgs_init3(&args, 3, 1, 18); break;
    case PATTERN_DOUBLEDOPS: PatternArgs_init5(&args, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, DOPS_OFF_DURATION, 0, 2); break;
    case PATTERN_CHOPPER: PatternArgs_init4(&args, ULTRADOPS_ON_DURATION, STROBIE_OFF_DURATION, 0, STROBE_ON_DURATION); break;
    case PATTERN_DASHGAP: PatternArgs_init4(&args, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, 20, 20); break;
    case PATTERN_DASHDOPS: PatternArgs_init4(&args, DOPS_ON_DURATION, DOPS_OFF_DURATION, DOPS_OFF_DURATION, 18); break;
    case PATTERN_DASHCRUSH: PatternArgs_init4(&args, 4, 1, 10, 18); break;
    case PATTERN_ULTRADASH: PatternArgs_init4(&args, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, ULTRADOPS_OFF_DURATION, 14); break;
    case PATTERN_GAPCYCLE: PatternArgs_init5(&args, STROBIE_ON_DURATION, STROBE_OFF_DURATION, 12, 25, 2); break;
    case PATTERN_DASHCYCLE: PatternArgs_init5(&args, ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, ULTRADOPS_OFF_DURATION, 30, 2); break;
    case PATTERN_TRACER: PatternArgs_init5(&args, 3, 0, 0, 20, 1); break;
    case PATTERN_RIBBON: PatternArgs_init1(&args, RIBBON_DURATION); break;
    case PATTERN_MINIRIBBON: PatternArgs_init1(&args, 1); break;
    case PATTERN_BLEND: PatternArgs_init7(&args, BLEND_ON_DURATION, BLEND_OFF_DURATION, 0, 0, 0, 5, 0); break;
    case PATTERN_BLENDSTROBE: PatternArgs_init7(&args, STROBE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 10, 0); break;
    case PATTERN_BLENDSTROBEGAP: PatternArgs_init7(&args, STROBE_ON_DURATION, STROBE_OFF_DURATION, 25, 0, 0, 10, 0); break;
    case PATTERN_COMPLEMENTARY_BLEND: PatternArgs_init7(&args, BLEND_ON_DURATION, BLEND_OFF_DURATION, 0, 0, 0, 5, 1); break;
    case PATTERN_COMPLEMENTARY_BLENDSTROBE: PatternArgs_init7(&args, STROBE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 10, 1); break;
    case PATTERN_COMPLEMENTARY_BLENDSTROBEGAP: PatternArgs_init7(&args, STROBE_ON_DURATION, STROBE_OFF_DURATION, 25, 0, 0, 10, 1); break;
    case PATTERN_SOLID: PatternArgs_init1(&args, 250); break;

#if VORTEX_SLIM == 0
    case PATTERN_HUE_SCROLL: PatternArgs_init3(&args, 1, 1, 10); break;
    case PATTERN_THEATER_CHASE: PatternArgs_init3(&args, DOPS_ON_DURATION, DOPS_OFF_DURATION, 28); break;
    case PATTERN_CHASER: PatternArgs_init(&args); break;
    case PATTERN_ZIGZAG: PatternArgs_init5(&args, DOPS_ON_DURATION, 3, 3, 5, 55); break;
    case PATTERN_ZIPFADE: PatternArgs_init5(&args, DOPS_ON_DURATION, 2, 75, 9, 230); break;
    case PATTERN_DRIP: PatternArgs_init3(&args, STROBE_ON_DURATION, STROBE_OFF_DURATION, 150); break;
    case PATTERN_DRIPMORPH: PatternArgs_init3(&args, STROBE_ON_DURATION, STROBE_OFF_DURATION, 1); break;
    case PATTERN_CROSSDOPS: PatternArgs_init3(&args, DOPS_ON_DURATION, 2, 25); break;
    case PATTERN_DOUBLESTROBE: PatternArgs_init3(&args, DOPS_ON_DURATION, DOPS_OFF_DURATION, 115); break;
    case PATTERN_METEOR: PatternArgs_init4(&args, 1, 1, 20, 130); break;
    case PATTERN_SPARKLETRACE: PatternArgs_init3(&args, 1, 5, 3); break;
    case PATTERN_VORTEXWIPE: PatternArgs_init3(&args, DOPS_ON_DURATION, 3, 80); break;
    case PATTERN_WARP: PatternArgs_init3(&args, 3, DOPS_OFF_DURATION, 50); break;
    case PATTERN_WARPWORM: PatternArgs_init3(&args, DOPS_ON_DURATION, 10, 100); break;
    case PATTERN_SNOWBALL: PatternArgs_init3(&args, 3, 3, 33); break;
    case PATTERN_LIGHTHOUSE: PatternArgs_init5(&args, DOPS_ON_DURATION, 5, 22, 3, 3); break;
    case PATTERN_PULSISH: PatternArgs_init5(&args, DOPS_ON_DURATION, 6, 5, 1, 100); break;
    case PATTERN_FILL: PatternArgs_init3(&args, DOPS_ON_DURATION, 5, 50); break;
    case PATTERN_BOUNCE: PatternArgs_init4(&args, 10, 5, 50, 10); break;
    case PATTERN_SPLITSTROBIE: PatternArgs_init8(&args, DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, 16, 3, 10, PATTERN_DOPS, PATTERN_STROBIE); break;
    case PATTERN_BACKSTROBE: PatternArgs_init8(&args, DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION, 10, PATTERN_DOPS, PATTERN_HYPERSTROBE); break;
    case PATTERN_VORTEX: PatternArgs_init3(&args, 1, 1, 130); break;
#else
    default: PatternArgs_init2(&args, DOPS_ON_DURATION, DOPS_OFF_DURATION); break;
#endif
  }
  return args;
}

uint8_t PatternBuilder_numDefaultArgs(PatternID id)
{
  Pattern *pat = PatternBuilder_make(id, NULL);
  if (!pat) {
    return 0;
  }
  uint8_t numArgs = Pattern_getNumArgs(pat);
  Pattern_destroy(pat);
  vfree(pat);
  return numArgs;
}

static Pattern *PatternBuilder_generate(PatternID id, const PatternArgs *userArgs)
{
  PatternArgs defaultArgs = PatternBuilder_getDefaultArgs(id);
  const PatternArgs *args = userArgs ? userArgs : &defaultArgs;

  switch (id) {
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
    case PATTERN_MINIRIBBON: {
      BasicPattern *bp = (BasicPattern *)vmalloc(sizeof(BasicPattern));
      if (!bp) return NULL;
      BasicPattern_init(bp, args);
      return &bp->base.base;
    }
    case PATTERN_BLEND:
    case PATTERN_BLENDSTROBE:
    case PATTERN_BLENDSTROBEGAP:
    case PATTERN_COMPLEMENTARY_BLEND:
    case PATTERN_COMPLEMENTARY_BLENDSTROBE:
    case PATTERN_COMPLEMENTARY_BLENDSTROBEGAP: {
      BlendPattern *bp = (BlendPattern *)vmalloc(sizeof(BlendPattern));
      if (!bp) return NULL;
      BlendPattern_init(bp, args);
      return &bp->base.base.base;
    }
    case PATTERN_SOLID: {
      SolidPattern *sp = (SolidPattern *)vmalloc(sizeof(SolidPattern));
      if (!sp) return NULL;
      SolidPattern_init(sp, args);
      return &sp->base.base.base;
    }

#if VORTEX_SLIM == 0
    case PATTERN_HUE_SCROLL: {
      HueShiftPattern *p = (HueShiftPattern *)vmalloc(sizeof(HueShiftPattern));
      if (!p) return NULL;
      HueShiftPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_THEATER_CHASE: {
      TheaterChasePattern *p = (TheaterChasePattern *)vmalloc(sizeof(TheaterChasePattern));
      if (!p) return NULL;
      TheaterChasePattern_init(p, args);
      return &p->base;
    }
    case PATTERN_CHASER: {
      ChaserPattern *p = (ChaserPattern *)vmalloc(sizeof(ChaserPattern));
      if (!p) return NULL;
      ChaserPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_ZIGZAG:
    case PATTERN_ZIPFADE: {
      ZigzagPattern *p = (ZigzagPattern *)vmalloc(sizeof(ZigzagPattern));
      if (!p) return NULL;
      ZigzagPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_DRIP: {
      DripPattern *p = (DripPattern *)vmalloc(sizeof(DripPattern));
      if (!p) return NULL;
      DripPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_DRIPMORPH: {
      DripMorphPattern *p = (DripMorphPattern *)vmalloc(sizeof(DripMorphPattern));
      if (!p) return NULL;
      DripMorphPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_CROSSDOPS: {
      CrossDopsPattern *p = (CrossDopsPattern *)vmalloc(sizeof(CrossDopsPattern));
      if (!p) return NULL;
      CrossDopsPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_DOUBLESTROBE: {
      DoubleStrobePattern *p = (DoubleStrobePattern *)vmalloc(sizeof(DoubleStrobePattern));
      if (!p) return NULL;
      DoubleStrobePattern_init(p, args);
      return &p->base;
    }
    case PATTERN_METEOR: {
      MeteorPattern *p = (MeteorPattern *)vmalloc(sizeof(MeteorPattern));
      if (!p) return NULL;
      MeteorPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_SPARKLETRACE: {
      SparkleTracePattern *p = (SparkleTracePattern *)vmalloc(sizeof(SparkleTracePattern));
      if (!p) return NULL;
      SparkleTracePattern_init(p, args);
      return &p->base;
    }
    case PATTERN_VORTEXWIPE: {
      VortexWipePattern *p = (VortexWipePattern *)vmalloc(sizeof(VortexWipePattern));
      if (!p) return NULL;
      VortexWipePattern_init(p, args);
      return &p->base;
    }
    case PATTERN_WARP: {
      WarpPattern *p = (WarpPattern *)vmalloc(sizeof(WarpPattern));
      if (!p) return NULL;
      WarpPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_WARPWORM: {
      WarpWormPattern *p = (WarpWormPattern *)vmalloc(sizeof(WarpWormPattern));
      if (!p) return NULL;
      WarpWormPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_SNOWBALL: {
      SnowballPattern *p = (SnowballPattern *)vmalloc(sizeof(SnowballPattern));
      if (!p) return NULL;
      SnowballPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_LIGHTHOUSE: {
      LighthousePattern *p = (LighthousePattern *)vmalloc(sizeof(LighthousePattern));
      if (!p) return NULL;
      LighthousePattern_init(p, args);
      return &p->base;
    }
    case PATTERN_PULSISH: {
      PulsishPattern *p = (PulsishPattern *)vmalloc(sizeof(PulsishPattern));
      if (!p) return NULL;
      PulsishPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_FILL: {
      FillPattern *p = (FillPattern *)vmalloc(sizeof(FillPattern));
      if (!p) return NULL;
      FillPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_BOUNCE: {
      BouncePattern *p = (BouncePattern *)vmalloc(sizeof(BouncePattern));
      if (!p) return NULL;
      BouncePattern_init(p, args);
      return &p->base;
    }
    case PATTERN_SPLITSTROBIE:
    case PATTERN_BACKSTROBE: {
      BackStrobePattern *p = (BackStrobePattern *)vmalloc(sizeof(BackStrobePattern));
      if (!p) return NULL;
      BackStrobePattern_init(p, args);
      return &p->base;
    }
    case PATTERN_VORTEX: {
      VortexPattern *p = (VortexPattern *)vmalloc(sizeof(VortexPattern));
      if (!p) return NULL;
      VortexPattern_init(p, args);
      return &p->base;
    }
    case PATTERN_NONE: return NULL;
#else
    case PATTERN_NONE: return NULL;
#endif
  }
  DEBUG_LOGF("Unknown pattern id: %u", id);
  return NULL;
}
