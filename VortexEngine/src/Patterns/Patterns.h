#ifndef PATTERNS_H
#define PATTERNS_H

#include <inttypes.h>

// List of patterns that can be built, both single and multi-led
// patterns are found in this list.
// Within both single and multi LED pattern lists there are 'core'
// patterns which are associated with a class, and there are 'shell'
// patterns which are simply wrapperns around another pattern with
// different parameters passed to the constructor.
// There is no way to know which patterns are 'core' patterns.
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

  PATTERN_STROBE = PATTERN_FIRST,
  PATTERN_HYPERSTROBE,
  PATTERN_DOPS,
  PATTERN_STROBIE,
  PATTERN_DOPISH,
  PATTERN_ULTRADOPS,
  PATTERN_STROBE2,
  PATTERN_HYPERSTROBE2,
  PATTERN_DOPS2,
  PATTERN_STROBIE2,
  PATTERN_DOPISH2,
  PATTERN_ULTRADOPS2,
  PATTERN_BLINKIE,
  PATTERN_GHOSTCRUSH,
  PATTERN_BASIC,
  PATTERN_BASIC2,
  PATTERN_BASIC3,
  PATTERN_BASIC4,
  PATTERN_BASIC5,
  PATTERN_BASIC6,
  PATTERN_BLEND,
  PATTERN_BLENDSTROBE,
  PATTERN_COMPLEMENTARY_BLEND,
  PATTERN_COMPLEMENTARY_BLENDSTROBE,
  PATTERN_DASHDOPS,
  PATTERN_DASHCRUSH,
  PATTERN_TRACER,
  PATTERN_RIBBON,
  PATTERN_MINIRIBBON,
  PATTERN_SOLID,

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

  PATTERN_HUESHIFT = PATTERN_MULTI_FIRST,
  PATTERN_THEATER_CHASE,
  PATTERN_CHASER,
  PATTERN_ZIGZAG,
  PATTERN_ZIPFADE,
  PATTERN_DRIP,
  PATTERN_DRIPMORPH,
  PATTERN_CROSSDOPS,
  PATTERN_DOUBLESTROBE,
  PATTERN_METEOR,
  PATTERN_SPARKLETRACE,
  PATTERN_VORTEXWIPE,
  PATTERN_WARP,
  PATTERN_WARPWORM,
  PATTERN_SNOWBALL,
  PATTERN_LIGHTHOUSE,
  PATTERN_PULSISH,
  PATTERN_FILL,
  PATTERN_BOUNCE,
  PATTERN_SPLITSTROBIE,
  PATTERN_BACKSTROBE,
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
inline bool isMultiLedPatternID(PatternID id) {
  return id >= PATTERN_MULTI_FIRST && id <= PATTERN_MULTI_LAST;
}
inline bool isSingleLedPatternID(PatternID id) {
  return id < PATTERN_MULTI_FIRST;
}

// PatternID operators
inline PatternID &operator++(PatternID &c)
{
  c = PatternID(((uint16_t)c) + 1);
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
  return (PatternID)((uint16_t)c + b);
}
inline PatternID operator-(PatternID &c, int b)
{
  return (PatternID)((uint16_t)c - b);
}

#endif
