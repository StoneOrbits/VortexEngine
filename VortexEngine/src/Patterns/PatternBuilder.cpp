#include "PatternBuilder.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"

#include "Multi/Sequencer/SequencedPattern.h"
#include "Multi/Sequencer/ChaserPattern.h"
#include "Multi/Sequencer/Sequence.h"

#include "Multi/RabbitPattern.h"

#include "Single/ComplementaryBlendPattern.h"
#include "Single/BracketsPattern.h"
#include "Single/AdvancedPattern.h"
#include "Single/TracerPattern.h"
#include "Single/DashDopsPattern.h"
#include "Single/SolidPattern.h"
#include "Single/BasicPattern.h"
#include "Single/BlendPattern.h"

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
PatternArgs PatternBuilder::getDefaultArgs(PatternID id)
{
  switch (id) {
    // =====================
    //  Single Led Patterns:
    case PATTERN_BASIC: return PatternArgs(1, 0, 0);
    case PATTERN_STROBE: return PatternArgs(STROBE_ON_DURATION, STROBE_OFF_DURATION);
    case PATTERN_HYPERSTROBE: return PatternArgs(HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION);
    case PATTERN_DOPS: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION);
    case PATTERN_DOPISH: return PatternArgs(DOPISH_ON_DURATION, DOPISH_OFF_DURATION);
    case PATTERN_ULTRADOPS: return PatternArgs(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION);
    case PATTERN_STROBIE: return PatternArgs(STROBIE_ON_DURATION, STROBE_OFF_DURATION);
    case PATTERN_RIBBON: return PatternArgs(RIBBON_DURATION);
    case PATTERN_MINIRIBBON: return PatternArgs(3);
    case PATTERN_BLINKIE: return PatternArgs(3, 6, 60);
    case PATTERN_GHOSTCRUSH: return PatternArgs(4, 1, 55);
    case PATTERN_SOLID: return PatternArgs(250, 0, 0, 0);
    case PATTERN_TRACER: return PatternArgs(16, 3);
    case PATTERN_DASHDOPS: return PatternArgs(30, 2, 7);
    case PATTERN_ADVANCED: return PatternArgs(5, 5, 10, 2, 2, 1);
    case PATTERN_BLEND: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, 0);
    case PATTERN_COMPLEMENTARY_BLEND: return PatternArgs(DOPS_ON_DURATION, DOPS_OFF_DURATION, 0, 0);
    case PATTERN_BRACKETS: return PatternArgs(2, 5, 8);

    // =====================
    //  Multi Led Patterns:
    case PATTERN_RABBIT: return PatternArgs(8, 10, 0, 6, 28, 0);
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
    case PATTERN_ADVANCED: return new AdvancedPattern(args);
    case PATTERN_BLEND: return new BlendPattern(args);
    case PATTERN_COMPLEMENTARY_BLEND: return new ComplementaryBlendPattern(args);
    case PATTERN_BRACKETS: return new BracketsPattern(args);

    // =====================
    //  Multi Led Patterns:
    case PATTERN_RABBIT: return new RabbitPattern(args);
    case PATTERN_NONE: return nullptr;
    default: break;
  }
  DEBUG_LOGF("Unknown pattern id: %u", id);
  return nullptr;
}
