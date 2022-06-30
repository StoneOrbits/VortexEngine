#ifndef PATTERNS_H
#define PATTERNS_H

#include <inttypes.h>

// list of patterns that can be built
enum PatternID : uint8_t
{
  // no pattern at all, use this sparingly and default to 
  // PATTERN_FIRST when possible 
  PATTERN_NONE = (PatternID)-1,

  // first pattern of all
  PATTERN_FIRST = 0,
  // first 'single led' pattern
  PATTERN_SINGLE_FIRST = PATTERN_FIRST,
  // =====================================

  // all 'single led' patterns below

  PATTERN_STROBE = PATTERN_FIRST,  // basicpattern 5 8 0
  PATTERN_SOLID,
  PATTERN_HYPERSTROBE, // basicpattern 25 25 0
  PATTERN_DOPS, // basicpattern 2 13 0
  PATTERN_DOPISH, // basicpattern 2 7 0
  PATTERN_ULTRADOPS, // basicpattern 1 3 0
  PATTERN_STROBIE, // basicpattern 3 22 0
  // these two don't change... why
  PATTERN_RIBBON, // basicpattern 20 0 0
  //PATTERN_MINIRIBBON, // basicpattern 3 0 0
  PATTERN_TRACER, // tracerpattern 20 1
  PATTERN_BLINKIE, // BasicPattern 5, 8, 35
  PATTERN_GHOSTCRUSH, // BasicPattern 1, 0, 50
  PATTERN_ADVANCED, // AdvancedPattern
  PATTERN_BLEND,  // BlendPattern 2, 13, 1
  PATTERN_RECIPROCAL_BLEND,  // ReciprocalBlendPattern 2, 13, 1
  PATTERN_BRACKETS,  // BracketsPattern

  // ADD NEW SINGLE LED PATTERNS HERE
  // WARNING This will offset all multi-led patterns so IR sharing
  //         of patterns will be misaligned. This means there will
  //         be inconsistencies sharing patterns over IR.

  // =====================================
  //  Pattern Meta Constants:
  PATTERN_MULTI_FIRST,
  PATTERN_SINGLE_LAST = (PATTERN_MULTI_FIRST - 1),
  PATTERN_SINGLE_COUNT = (PATTERN_SINGLE_LAST - PATTERN_SINGLE_FIRST) + 1,
  // =====================================

  // all 'multi led' patterns below

  PATTERN_RABBIT = PATTERN_MULTI_FIRST, // PATTERN_STROBIE on tops and PATTERN_STROBE on tips
  PATTERN_HUESHIFT,
  PATTERN_THEATER_CHASE,
  PATTERN_CHASER,

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
