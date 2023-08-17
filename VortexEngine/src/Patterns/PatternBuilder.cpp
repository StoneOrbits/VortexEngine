#include "PatternBuilder.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"

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
#include "Multi/MateriaPattern.h"

#include "Single/SingleLedPattern.h"
#include "Single/BasicPattern.h"
#include "Single/BlendPattern.h"
#include "Single/SolidPattern.h"

Pattern *PatternBuilder::make(PatternID id, const PatternArgs *args)
{
#if VORTEX_SLIM == 0
  if (id > PATTERN_LAST) {
#else
  if (id > PATTERN_SINGLE_LAST) {
#endif
    DEBUG_LOGF("Invalid pattern id: %u", id);
    id = PATTERN_FIRST;
  }
  if (isMultiLedPatternID(id)) {
    return makeMulti(id, args);
  }
  return makeSingle(id, args);
}

Pattern *PatternBuilder::dupe(const Pattern *pat)
{
  if (!pat) {
    return nullptr;
  }
  PatternArgs args;
  pat->getArgs(args);
  Pattern *newPat = make(pat->getPatternID(), &args);
  if (!newPat) {
    return nullptr;
  }
  newPat->setColorset(pat->getColorset());
  newPat->bind(pat->getLedPos());
  return newPat;
}

// generate a single LED pattern (nullptr if patternid is not single LED)
SingleLedPattern *PatternBuilder::makeSingle(PatternID id, const PatternArgs *args)
{
  if (!isSingleLedPatternID(id)) {
    return nullptr;
  }
  Pattern *pat = makeInternal(id, args);
  if (!pat) {
    return nullptr;
  }
  // don't set any flags on single pattersn
  return (SingleLedPattern *)pat;
}

// generate a multi LED pattern (nullptr if patternid is not multi LED)
MultiLedPattern *PatternBuilder::makeMulti(PatternID id, const PatternArgs *args)
{
  if (!isMultiLedPatternID(id)) {
    return nullptr;
  }
  Pattern *pat = makeInternal(id, args);
  if (!pat) {
    return nullptr;
  }
  return (MultiLedPattern *)pat;
}

Pattern *PatternBuilder::unserialize(ByteStream &buffer)
{
  Pattern *pat = make((PatternID)buffer.unserialize8());
  if (!pat) {
    return nullptr;
  }
  pat->unserialize(buffer);
  return pat;
}

Pattern *PatternBuilder::makeInternal(PatternID id, const PatternArgs *args)
{
  if (id == PATTERN_NONE) {
    // no error
    return nullptr;
  }
  Pattern *pat = generate(id, args);
  if (!pat) {
    ERROR_OUT_OF_MEMORY();
    return nullptr;
  }
  // set private pattern ID via friend class relationship
  pat->m_patternID = id;
  return pat;
}

// This is just the default arguments for any given pattern id
// it will *not* indicate the true amount of arguments a pattern has
// Some pattern ids are purely defined by their unique arguments
// to a class, so calling them 'default' might be misleading
PatternArgs PatternBuilder::getDefaultArgs(PatternID id)
{
  switch (id) {
    // =====================
    //  Single Led Patterns:
    case PATTERN_STROBE: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION);
    case PATTERN_HYPERSTROBE: return PatternArgs(HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION);
    case PATTERN_PICOSTROBE: return PatternArgs(PICOSTROBE_ON_DURATION, PICOSTROBE_OFF_DURATION);
    case PATTERN_STROBIE: return PatternArgs(STROBIE_ON_DURATION, STROBIE_OFF_DURATION);
    case PATTERN_DOPS: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION);
    case PATTERN_ULTRADOPS: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION);
    case PATTERN_STROBEGAP: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 25);
    case PATTERN_HYPERGAP: return PatternArgs(HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION, 218);
    case PATTERN_PICOGAP: return PatternArgs(PICOSTROBE_ON_DURATION, PICOSTROBE_OFF_DURATION, 90);
    case PATTERN_STROBIEGAP: return PatternArgs(STROBIE_ON_DURATION, STROBIE_OFF_DURATION, 100);
    case PATTERN_DOPSGAP: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 40);
    case PATTERN_ULTRAGAP: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, 55);
    case PATTERN_BLINKIE: return PatternArgs(3, 1, 65);
    case PATTERN_GHOSTCRUSH: return PatternArgs(3, 1, 18);
    case PATTERN_DOUBLEDOPS: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, DOPS_OFF_DURATION, 0, 2);
    case PATTERN_CHOPPER: return PatternArgs(ULTRADOPS_ON_DURATION, STROBIE_OFF_DURATION, 0, STROBE_ON_DURATION);
    case PATTERN_DASHGAP: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, 20, 20);
    case PATTERN_DASHDOPS: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, DOPS_OFF_DURATION, 18);
    case PATTERN_DASHCRUSH: return PatternArgs(4, 1, 10, 18);
    case PATTERN_ULTRADASH: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, ULTRADOPS_OFF_DURATION, 14);
    case PATTERN_GAPCYCLE: return PatternArgs(ULTRADOPS_ON_DURATION, STROBIE_OFF_DURATION, 20, 40, 2);
    case PATTERN_DASHCYCLE: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, ULTRADOPS_OFF_DURATION, 35, 2);
    case PATTERN_TRACER: return PatternArgs(ULTRADOPS_ON_DURATION, 0, 0, 20, 1);
    case PATTERN_RIBBON: return PatternArgs(RIBBON_DURATION);
    case PATTERN_MINIRIBBON: return PatternArgs(1);
    case PATTERN_BLEND: return PatternArgs(BLEND_ON_DURATION, BLEND_OFF_DURATION, 0, 0, 0, 2, 1);
    case PATTERN_BLENDSTROBE: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 0, 1);
    case PATTERN_BLENDSTROBEGAP: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 75, 0, 0, 9, 1);
    case PATTERN_COMPLEMENTARY_BLEND: return PatternArgs(BLEND_ON_DURATION, BLEND_OFF_DURATION, 0, 0, 0, 2, 2);
    case PATTERN_COMPLEMENTARY_BLENDSTROBE: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 0, 2);
    case PATTERN_COMPLEMENTARY_BLENDSTROBEGAP: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 75, 0, 0, 9, 2);
    case PATTERN_SOLID: return PatternArgs(250);

    // =====================
    //  Multi Led Patterns:
#if VORTEX_SLIM == 0
    case PATTERN_HUESHIFT: return PatternArgs(1, 1);
    case PATTERN_THEATER_CHASE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 28);
    case PATTERN_CHASER: return PatternArgs();
    case PATTERN_ZIGZAG: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 55, 1, 55);
    case PATTERN_ZIPFADE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 100, 4, 1);
    case PATTERN_DRIP: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 250);
    case PATTERN_DRIPMORPH: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 1);
    case PATTERN_CROSSDOPS: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 100);
    case PATTERN_DOUBLESTROBE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 115);
    case PATTERN_METEOR: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 55, 75);
    case PATTERN_SPARKLETRACE: return PatternArgs(5, 0, 50);
    case PATTERN_VORTEXWIPE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 130);
    case PATTERN_WARP: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 150);
    case PATTERN_WARPWORM: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 110);
    case PATTERN_SNOWBALL: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 110);
    case PATTERN_LIGHTHOUSE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 100, 25, 5);
    case PATTERN_PULSISH: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, STROBE_ON_DURATION, STROBE_OFF_DURATION, 250);
    case PATTERN_FILL: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 200);
    case PATTERN_BOUNCE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 200, 10);
    case PATTERN_SPLITSTROBIE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, 16, 3, 10, PATTERN_DOPS, PATTERN_STROBIE);
    case PATTERN_BACKSTROBE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION, 10, PATTERN_DOPS, PATTERN_HYPERSTROBE);
    case PATTERN_MATERIA: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 3, 35, 80);
    case PATTERN_NONE: break;
    default: break;
#else
    // in vortex slim just use DOPS for all mult-led
    case PATTERN_NONE: break;
    default: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION);
#endif
  }
  return PatternArgs();
}

uint8_t PatternBuilder::numDefaultArgs(PatternID id)
{
  Pattern *pat = make(id);
  if (!pat) {
    return 0;
  }
  uint8_t numArgs = pat->getNumArgs();
  delete pat;
  return numArgs;
}

// This is responsible for instantiating the basic core patterns
Pattern *PatternBuilder::generate(PatternID id, const PatternArgs *userArgs)
{
  PatternArgs args = userArgs ? *userArgs : getDefaultArgs(id);
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
    case PATTERN_MINIRIBBON: return new BasicPattern(args);
    case PATTERN_BLEND:
    case PATTERN_BLENDSTROBE:
    case PATTERN_BLENDSTROBEGAP:
    case PATTERN_COMPLEMENTARY_BLEND:
    case PATTERN_COMPLEMENTARY_BLENDSTROBE:
    case PATTERN_COMPLEMENTARY_BLENDSTROBEGAP: return new BlendPattern(args);
    case PATTERN_SOLID: return new SolidPattern(args);

    // =====================
    //  Multi Led Patterns:
#if VORTEX_SLIM == 0
    case PATTERN_HUESHIFT: return new HueShiftPattern(args);
    case PATTERN_THEATER_CHASE: return new TheaterChasePattern(args);
    case PATTERN_CHASER: return new ChaserPattern(args);
    case PATTERN_ZIGZAG:
    case PATTERN_ZIPFADE: return new ZigzagPattern(args);
    case PATTERN_DRIP: return new DripPattern(args);
    case PATTERN_DRIPMORPH: return new DripMorphPattern(args);
    case PATTERN_CROSSDOPS: return new CrossDopsPattern(args);
    case PATTERN_DOUBLESTROBE: return new DoubleStrobePattern(args);
    case PATTERN_METEOR: return new MeteorPattern(args);
    case PATTERN_SPARKLETRACE: return new SparkleTracePattern(args);
    case PATTERN_VORTEXWIPE: return new VortexWipePattern(args);
    case PATTERN_WARP: return new WarpPattern(args);
    case PATTERN_WARPWORM: return new WarpWormPattern(args);
    case PATTERN_SNOWBALL: return new SnowballPattern(args);
    case PATTERN_LIGHTHOUSE: return new LighthousePattern(args);
    case PATTERN_PULSISH: return new PulsishPattern(args);
    case PATTERN_FILL: return new FillPattern(args);
    case PATTERN_BOUNCE: return new BouncePattern(args);
    case PATTERN_SPLITSTROBIE:
    case PATTERN_BACKSTROBE: return new BackStrobePattern(args);
    case PATTERN_MATERIA: return new MateriaPattern(args);
    case PATTERN_NONE: return nullptr;
#else
    // in vortex slim just use basic pattern for all multi led
    case PATTERN_NONE: return nullptr;
#endif
  }
  DEBUG_LOGF("Unknown pattern id: %u", id);
  return nullptr;
}
