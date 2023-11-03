#include "PatternBuilder.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"

#include "Single/BasicPattern.h"
#include "Single/BlendPattern.h"

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
  return makeInternal(id, args);
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
    case PATTERN_GAPCYCLE: return PatternArgs(STROBIE_ON_DURATION, STROBE_OFF_DURATION, 12, 25, 2);
    case PATTERN_DASHCYCLE: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, ULTRADOPS_OFF_DURATION, 30, 2);
    case PATTERN_TRACER: return PatternArgs(3, 0, 0, 20, 1);
    case PATTERN_RIBBON: return PatternArgs(RIBBON_DURATION);
    case PATTERN_MINIRIBBON: return PatternArgs(1);
    case PATTERN_BLEND: return PatternArgs(BLEND_ON_DURATION, BLEND_OFF_DURATION, 0, 0, 0, 5, 1);
    case PATTERN_BLENDSTROBE: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 10, 1);
    case PATTERN_BLENDSTROBEGAP: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 25, 0, 0, 10, 1);
    case PATTERN_COMPLEMENTARY_BLEND: return PatternArgs(BLEND_ON_DURATION, BLEND_OFF_DURATION, 0, 0, 0, 5, 2);
    case PATTERN_COMPLEMENTARY_BLENDSTROBE: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 0, 0, 0, 10, 2);
    case PATTERN_COMPLEMENTARY_BLENDSTROBEGAP: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 25, 0, 0, 10, 2);
    case PATTERN_SOLID: return PatternArgs(250);

    // =====================
    //  Multi Led Patterns:
#if VORTEX_SLIM == 0
    case PATTERN_HUE_SCROLL: return PatternArgs(1, 1);
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
  if (id >= PATTERN_MINIRIBBON) {
    return new BlendPattern(args);
  }
  return new BasicPattern(args);
}
