#include "PatternBuilder.h"

#include "TimeControl.h"

#include "patterns/AdvancedPattern.h"
#include "patterns/BasicPattern.h"
#include "patterns/TracerPattern.h"
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

Pattern *PatternBuilder::generate(PatternID id)
{
  // NOTE: The timings of patterns are only defined at creation time
  //       which means altering the tickrate will not change how fast
  //       a pattern displays unless you re-create it
  switch (id) {
  default:
  case PATTERN_STROBE:
    return new AdvancedPattern(5, 8, 30, 2, 0, 0);
    //return new BasicPattern(5, 8);
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
  }
}
