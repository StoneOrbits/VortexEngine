#include "PatternBuilder.h"

#include "TimeControl.h"

#include "patterns/AdvancedPattern.h"
#include "patterns/RabbitPattern.h"
#include "patterns/TracerPattern.h"
#include "patterns/BasicPattern.h"
#include "patterns/GapPattern.h"

Pattern *PatternBuilder::make(PatternID id)
{
  Pattern *pat = generate(id);
  if (!pat) {
    return nullptr;
  }
  // set private pattern ID via friend class relationship
  pat->m_patternID = id;
  return pat;
}

// generate a single LED pattern (nullptr if patternid is not single LED)
SingleLedPattern *PatternBuilder::makeSingle(PatternID id)
{
  Pattern *pat = make(id);
  if (!pat) {
    return nullptr;
  }
  // don't set any flags on single pattersn
}

// generate a multi LED pattern (nullptr if patternid is not multi LED)
MultiLedPattern *PatternBuilder::makeMulti(PatternID id)
{
  Pattern *pat = make(id);
  if (!pat) {
    return nullptr;
  }
  // set the multi flag on multi led patterns
  pat->m_patternFlags |= PATTERN_FLAG_MULTI;
}

Pattern *PatternBuilder::generate(PatternID id)
{
  // NOTE: The timings of patterns are only defined at creation time
  //       which means altering the tickrate will not change how fast
  //       a pattern displays unless you re-create it
  switch (id) {
  default:
  case PATTERN_NONE:
    // programmer error
    return nullptr;
  case PATTERN_STROBE:
    return new BasicPattern(20, 1);
  case PATTERN_HYPERSTROBE:
    return new BasicPattern(25, 25);
  case PATTERN_DOPS:
    return new BasicPattern(2, 13);
  case PATTERN_DOPISH:
    return new BasicPattern(2, 7);
  case PATTERN_ULTRADOPS:
    return new BasicPattern(1, 3);
  case PATTERN_STROBIE:
    return new BasicPattern(3, 22);
#if 0
  case PATTERN_RIBBON:
    return new BasicPattern(20);
  case PATTERN_MINIRIBBON:
    return new BasicPattern(3);
#endif
  case PATTERN_TRACER:
    return new TracerPattern(20, 1);
  case PATTERN_BLINKIE:
    return new GapPattern(5, 8, 35);
  case PATTERN_GHOSTCRUSH:
    return new GapPattern(1, 0, 50);
  case PATTERN_ADVANCED:
    // This advanced pattern demonstrates the functionality of PatternTheory
    return new AdvancedPattern(
      5,  // on duration
      5,  // off duration
      10, // gap duration
      2,  // group size (number of colors before gap)
      2,  // skip (number of colors to skip after gap)
      1   // repeat (number of times to repeat group)
    );
  case PATTERN_RABBIT:
    return new RabbitPattern();
  }
}
