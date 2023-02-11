#include "PatternBuilder.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"

#include "Single/AdvancedPattern.h"
#include "Single/BlendPattern.h"

Pattern *PatternBuilder::make(PatternID id, const PatternArgs *args)
{
  if (id > PATTERN_LAST) {
    DEBUG_LOGF("Invalid pattern id: %u", id);
    return nullptr;
  }
  Pattern *pat = makeInternal(id, args);
  if (!pat) {
    return nullptr;
  }
  return pat;
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
    case PATTERN_BASIC: return PatternArgs(1, 0, 0);
    case PATTERN_STROBE: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION, 0);
    case PATTERN_HYPERSTROBE: return PatternArgs(HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION, 0);
    case PATTERN_DOPS: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0);
    case PATTERN_DOPISH: return PatternArgs(DOPISH_ON_DURATION, DOPISH_OFF_DURATION, 0);
    case PATTERN_ULTRADOPS: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION, 0);
    case PATTERN_STROBIE: return PatternArgs(STROBIE_ON_DURATION, STROBE_OFF_DURATION, 0);
    case PATTERN_RIBBON: return PatternArgs(RIBBON_DURATION, 0, 0);
    case PATTERN_MINIRIBBON: return PatternArgs(3, 0, 0);
    case PATTERN_BLINKIE: return PatternArgs(3, 6, 60);
    case PATTERN_GHOSTCRUSH: return PatternArgs(4, 1, 55);
    case PATTERN_SOLID: return PatternArgs(250, 0, 0, 0);
    case PATTERN_TRACER: return PatternArgs(16, 3);
    case PATTERN_DASHDOPS: return PatternArgs(30, 2, 7);
    case PATTERN_ADVANCED: return PatternArgs(5, 5, 10, 2, 2, 1);
    case PATTERN_BLEND: return PatternArgs(2, 13, 0, 0, 0, 0, 0, 2);
    case PATTERN_COMPLEMENTARY_BLEND: return PatternArgs(5, 5, 10, 2, 2, 1, 0, 3);
    case PATTERN_BRACKETS: return PatternArgs(4, 8, 35);
    case PATTERN_NONE: break;
    default: break;
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
    default:
    case PATTERN_ADVANCED: return new AdvancedPattern(args);
    case PATTERN_BLEND: return new BlendPattern(args);
  }
  DEBUG_LOGF("Unknown pattern id: %u", id);
  return nullptr;
}
