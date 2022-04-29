#ifndef PATTERNS_H
#define PATTERNS_H

#include <inttypes.h>

// list of patterns that can be built
enum PatternID : uint32_t
{
  PATTERN_FIRST,

  PATTERN_STROBE = PATTERN_FIRST,  // basicpattern 5 8
  PATTERN_HYPERSTROBE, // basicpattern 25 25
  PATTERN_DOPS, // basicpattern 2 13
  PATTERN_DOPISH, // basicpattern 2 7
  PATTERN_ULTRADOPS, // basicpattern 1 3
  PATTERN_STROBIE, // basicpattern 3 22

  // these two don't change... why
  //PATTERN_RIBBON, // basicpattern 20
  //PATTERN_MINIRIBBON, // basicpattern 3

  PATTERN_TRACER, // tracerpattern 20 1

  // ADD NEW PATTERNS HERE

  PATTERN_COUNT, // total number of patterns
  PATTERN_LAST = (PATTERN_COUNT - 1)
};

// PatternID operators
inline PatternID &operator++(PatternID &c)
{
  c = PatternID(((uint32_t)c) + 1);
  return c;
}
inline PatternID operator++(PatternID &c, int)
{
  PatternID temp = c;
  ++c;
  return temp;
}
inline PatternID operator+(PatternID &c, int b)
{
  return (PatternID)((uint32_t)c + b);
}
inline PatternID operator-(PatternID &c, int b)
{
  return (PatternID)((uint32_t)c - b);
}

#endif
