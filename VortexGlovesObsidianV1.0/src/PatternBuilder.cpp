#include "PatternBuilder.h"

#include "TimeControl.h"

#include "patterns/BasicPattern.h"
#include "patterns/TracerPattern.h"

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
    return new BasicPattern(MS_TO_TICKS(5), MS_TO_TICKS(8));
  case PATTERN_HYPERSTROBE:
    return new BasicPattern(MS_TO_TICKS(25), MS_TO_TICKS(25));
  case PATTERN_DOPS:
    return new BasicPattern(MS_TO_TICKS(2), MS_TO_TICKS(13));
  case PATTERN_DOPISH:
    return new BasicPattern(MS_TO_TICKS(2), MS_TO_TICKS(7));
  case PATTERN_ULTRADOPS:
    return new BasicPattern(MS_TO_TICKS(1), MS_TO_TICKS(3));
  case PATTERN_STROBIE:
    return new BasicPattern(MS_TO_TICKS(3), MS_TO_TICKS(22));
  case PATTERN_RIBBON:
    return new BasicPattern(MS_TO_TICKS(20));
  case PATTERN_MINIRIBBON:
    return new BasicPattern(MS_TO_TICKS(3));
  case PATTERN_TRACER:
    return new TracerPattern(MS_TO_TICKS(20), MS_TO_TICKS(1));
  }
}
