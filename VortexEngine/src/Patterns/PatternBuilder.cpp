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

#include "Single/BracketsPattern.h"
#include "Single/TracerPattern.h"
#include "Single/DashDopsPattern.h"
#include "Single/BasicPattern.h"
#include "Single/BlendPattern.h"
#include "Single/SolidPattern.h"

Pattern *PatternBuilder::make(PatternID id, const PatternArgs *args)
{
  if (id > PATTERN_LAST) {
    DEBUG_LOGF("Invalid pattern id: %u", id);
    return nullptr;
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

// if your pattern ID wraps a core pattern with custom args then define
// those custom args here in this function
//
// NOTE: The number of args in each constructor here should match *exactly*
//       to the number of args in the respective constructor it will call
//       so for example PATTERN_BASIC should have 3 just like BasicPattern()
PatternArgs PatternBuilder::getDefaultArgs(PatternID id)
{
  switch (id) {
    // =====================
    //  Single Led Patterns:
    case PATTERN_BASIC: return PatternArgs(1, 0, 0, 0, 0, 0);
    case PATTERN_STROBE: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 0);
    case PATTERN_HYPERSTROBE: return PatternArgs(HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION, 0, 0, 0, 0);
    case PATTERN_DOPS: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, 0, 0, 0);
    case PATTERN_DOPISH: return PatternArgs(DOPISH_ON_DURATION, DOPISH_OFF_DURATION, 0, 0, 0, 0);
    case PATTERN_ULTRADOPS: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, 0, 0, 0, 0);
    case PATTERN_STROBIE: return PatternArgs(STROBIE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 0);
    case PATTERN_RIBBON: return PatternArgs(RIBBON_DURATION, 0, 0, 0, 0, 0);
    case PATTERN_MINIRIBBON: return PatternArgs(3, 0, 0, 0, 0, 0);
    case PATTERN_BLINKIE: return PatternArgs(3, 6, 60, 0, 0, 0);
    case PATTERN_GHOSTCRUSH: return PatternArgs(4, 1, 55, 0, 0, 0);
    case PATTERN_SOLID: return PatternArgs(250, 0, 0, 0, 0, 0, 0);
    case PATTERN_TRACER: return PatternArgs(16, 3);
    case PATTERN_DASHDOPS: return PatternArgs(30, 2, 7);
    case PATTERN_BLEND: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, 0, 0, 0, 0, 2);
    case PATTERN_COMPLEMENTARY_BLEND: return PatternArgs(5, 5, 10, 2, 2, 1, 0, 3);
    case PATTERN_BRACKETS: return PatternArgs(4, 8, 35);

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
    case PATTERN_VORTEXWIPE: return PatternArgs(DOPISH_ON_DURATION, DOPISH_OFF_DURATION, 130);
    case PATTERN_WARP: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 150);
    case PATTERN_WARPWORM: return PatternArgs(DOPISH_ON_DURATION, DOPISH_OFF_DURATION, 110);
    case PATTERN_SNOWBALL: return PatternArgs(DOPISH_ON_DURATION, DOPISH_OFF_DURATION, 110);
    case PATTERN_LIGHTHOUSE: return PatternArgs(DOPISH_ON_DURATION, DOPISH_OFF_DURATION, 100, 25, 5);
    case PATTERN_PULSISH: return PatternArgs(DOPISH_ON_DURATION, DOPISH_OFF_DURATION, STROBE_ON_DURATION, STROBE_OFF_DURATION, 250);
    case PATTERN_FILL: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 200);
    case PATTERN_BOUNCE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 200, 10);
    case PATTERN_SPLITSTROBIE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, 16, 3, 10, PATTERN_DOPS, PATTERN_TRACER);
    case PATTERN_BACKSTROBE: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION, 10, PATTERN_DOPS, PATTERN_HYPERSTROBE);
    case PATTERN_MATERIA: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 3, 35, 80);
    case PATTERN_NONE: break;
    default: break;
#else
    // in vortex slim just use DOPS for all mult-led
    case PATTERN_NONE: break;
    default: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, 0, 0, 0);
#endif
  }
  return PatternArgs();
}

// This is responsible for instantiating the basic core patterns
Pattern *PatternBuilder::generate(PatternID id, const PatternArgs *userArgs)
{
  PatternArgs args = userArgs ? *userArgs : getDefaultArgs(id);
  switch (id) {
    // =====================
    //  Single Led Patterns:
    case PATTERN_BASIC:
    case PATTERN_STROBE:
    case PATTERN_HYPERSTROBE:
    case PATTERN_DOPS:
    case PATTERN_DOPISH:
    case PATTERN_ULTRADOPS:
    case PATTERN_STROBIE:
    case PATTERN_RIBBON:
    case PATTERN_MINIRIBBON:
    case PATTERN_BLINKIE:
    case PATTERN_GHOSTCRUSH: return new BasicPattern(args);
    case PATTERN_SOLID: return new SolidPattern(args);
    case PATTERN_TRACER: return new TracerPattern(args);
    case PATTERN_DASHDOPS: return new DashDopsPattern(args);
    case PATTERN_BLEND:
    case PATTERN_COMPLEMENTARY_BLEND: return new BlendPattern(args);
    case PATTERN_BRACKETS: return new BracketsPattern(args);

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
    default: break;
#else
    // in vortex slim just use basic pattern for all multi led
    case PATTERN_NONE: return nullptr;
    default: return new BasicPattern(args);
#endif
  }
  DEBUG_LOGF("Unknown pattern id: %u", id);
  return nullptr;
}
