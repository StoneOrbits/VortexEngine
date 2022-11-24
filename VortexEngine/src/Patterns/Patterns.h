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
  PATTERN_HYPERSTROBE, // basicpattern 25 25 0
  PATTERN_DOPS, // basicpattern 2 13 0
  PATTERN_DOPISH, // basicpattern 2 7 0
  PATTERN_ULTRADOPS, // basicpattern 1 3 0
  PATTERN_STROBIE, // basicpattern 3 22 0
  PATTERN_RIBBON, // basicpattern 20 0 0
  PATTERN_MINIRIBBON, // basicpattern 3 0 0
  PATTERN_TRACER, // tracerpattern 20 1
  PATTERN_DASHDOPS, // dashDopsPattern 1 5 20
  PATTERN_BLINKIE, // BasicPattern 5, 8, 35
  PATTERN_GHOSTCRUSH, // BasicPattern 1, 0, 50
  PATTERN_ADVANCED, // AdvancedPattern
  PATTERN_BLEND,  // BlendPattern 2, 13, 1
  PATTERN_COMPLEMENTARY_BLEND,  // ComplementaryBlendPattern 2, 13, 1
  PATTERN_BRACKETS,  // BracketsPattern

  // solid patterns are mainly used as components of complex patterns
  PATTERN_SOLID0, // solid of 0th color in colorset
  PATTERN_SOLID1, // solid of 1st color in colorset
  PATTERN_SOLID2, // solid of 2nd color in colorset

  // ADD NEW SINGLE LED PATTERNS HERE
  // WARNING This will offset all multi-led patterns so sharing of patterns 
  //         will be misaligned, for ex. IR and save files. 

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
  PATTERN_ZIGZAG,
  PATTERN_ZIPFADE,
  PATTERN_TIPTOP,
  PATTERN_DRIP,
  PATTERN_DRIPMORPH,
  PATTERN_CROSSDOPS,
  PATTERN_DOUBLESTROBE,// doubleStrobe(5, 8, 75)
  // TODO: SLoth Strobe doubleStrobe(5, 8, 1000)
  PATTERN_METEOR,
  PATTERN_SPARKLETRACE,
  PATTERN_VORTEXWIPE, // vortexWipe(2,7,125)
  // TODO: UltraWipe vortexWipe(2, 7, 50)
  PATTERN_WARP,
  PATTERN_WARPWORM,
  PATTERN_SNOWBALL,
  PATTERN_LIGHTHOUSE,
  PATTERN_PULSISH,
  PATTERN_FILL,
  PATTERN_BOUNCE,
  PATTERN_IMPACT,
  PATTERN_SPLITSTROBIE,
  PATTERN_BACKSTROBE,
  PATTERN_FLOWERS,
  PATTERN_JEST,
  PATTERN_MATERIA,

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
