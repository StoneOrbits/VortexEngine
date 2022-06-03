#ifndef PATTERNS_H
#define PATTERNS_H

#include <inttypes.h>

// list of patterns that can be built
enum PatternID : uint8_t
{
  // first pattern of all
  PATTERN_FIRST = 0,
  // first 'single led' pattern
  PATTERN_SINGLE_FIRST = PATTERN_FIRST,
  // =====================================

  // all 'single led' patterns below

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
  PATTERN_BLINKIE, // GapPattern 5, 8, 35
  PATTERN_GHOSTCRUSH, // GapPattern 1, 0, 50
  PATTERN_ADVANCED, // AdvancedPattern
  PATTERN_BLEND,  // BlendPattern 2, 13, 1

  // ADD NEW SINGLE LED PATTERNS HERE

  // =====================================
  //  Pattern Meta Constants:
  PATTERN_MULTI_FIRST,
  PATTERN_SINGLE_LAST = (PATTERN_MULTI_FIRST - 1),
  PATTERN_SINGLE_COUNT = (PATTERN_SINGLE_LAST - PATTERN_SINGLE_FIRST) + 1,
  // =====================================

  // all 'multi led' patterns below

  PATTERN_RABBIT = PATTERN_MULTI_FIRST, // PATTERN_STROBIE on tops and PATTERN_STROBE on tips
  PATTERN_HUESHIFT,

  // ADD NEW MULTI LED PATTERNS HERE

  // =====================================
  INTERNAL_PATTERNS_END, // <<< DON'T USE OR TOUCH THIS ONE
  PATTERN_MULTI_LAST = (INTERNAL_PATTERNS_END - 1),
  PATTERN_MULTI_COUNT = (PATTERN_MULTI_LAST - PATTERN_MULTI_FIRST) + 1,
  PATTERN_LAST = PATTERN_MULTI_LAST,
  PATTERN_COUNT = (PATTERN_LAST - PATTERN_FIRST) + 1, // total number of patterns
};

// some helper functions to improve readability
inline bool isMultiLedPatternID(PatternID id) { return id >= PATTERN_MULTI_FIRST; }
inline bool isSingleLedPatternID(PatternID id) { return id < PATTERN_MULTI_FIRST; }

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
