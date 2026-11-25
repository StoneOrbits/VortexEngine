#ifndef PATTERNS_H
#define PATTERNS_H

#include <inttypes.h>

// List of patterns that can be built, both single and multi-led patterns are found in this list.
// Within both single and multi LED pattern lists there are 'core' patterns which are associated
// with a class, and there are 'shell' patterns which are simply wrapperns around another pattern
// with different parameters passed to the constructor.  There is no way to know which patterns
// are 'core' patterns, except by looking at PatternBuilder::generate to see which classes exist
enum PatternID : int8_t
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
  PATTERN_PICOSTROBE,
  PATTERN_STROBIE,
  PATTERN_DOPS,
  PATTERN_ULTRADOPS,
  PATTERN_STROBEGAP,
  PATTERN_HYPERGAP,
  PATTERN_PICOGAP,
  PATTERN_STROBIEGAP,
  PATTERN_DOPSGAP,
  PATTERN_ULTRAGAP,
  PATTERN_BLINKIE,
  PATTERN_GHOSTCRUSH,
  PATTERN_DOUBLEDOPS,
  PATTERN_CHOPPER,
  PATTERN_DASHGAP,
  PATTERN_DASHDOPS,
  PATTERN_DASHCRUSH,
  PATTERN_ULTRADASH,
  PATTERN_GAPCYCLE,
  PATTERN_DASHCYCLE,
  PATTERN_TRACER,
  PATTERN_RIBBON,
  PATTERN_MINIRIBBON,
  PATTERN_BLEND,
  PATTERN_BLENDSTROBE,
  PATTERN_BLENDSTROBEGAP,
  PATTERN_COMPLEMENTARY_BLEND,
  PATTERN_COMPLEMENTARY_BLENDSTROBE,
  PATTERN_COMPLEMENTARY_BLENDSTROBEGAP,
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

  PATTERN_HUE_SCROLL = PATTERN_MULTI_FIRST,
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
  PATTERN_VORTEX,

  // Vortex POV textures, these are all played by the PovPattern and each id
  // selects a texture from PovTextures.h (keep both lists in the same order)
  PATTERN_POV_FIRST,
  PATTERN_POV_ZIGZAG = PATTERN_POV_FIRST,
  PATTERN_POV_SINE,
  PATTERN_POV_HELIX,
  PATTERN_POV_CHECKER,
  PATTERN_POV_DIAMONDS,
  PATTERN_POV_CHEVRONS,
  PATTERN_POV_HEARTS,
  PATTERN_POV_STARS,
  PATTERN_POV_BRICKS,
  PATTERN_POV_DIAGONALS,
  PATTERN_POV_EQUALIZER,
  PATTERN_POV_DOTS,
  PATTERN_POV_TEXT,
  PATTERN_POV_LADDER,
  PATTERN_POV_STAIRS,
  PATTERN_POV_BUNTING,
  PATTERN_POV_SCALES,
  PATTERN_POV_CROSSES,
  PATTERN_POV_NAVAJO,
  PATTERN_POV_DAZZLER,
  PATTERN_POV_ARROWS,
  PATTERN_POV_MESA,
  PATTERN_POV_LIGHTNING,
  PATTERN_POV_FEATHERS,
  PATTERN_POV_GREEKKEY,
  PATTERN_POV_DENTILS,
  PATTERN_POV_EGGDART,
  PATTERN_POV_LAUREL,
  PATTERN_POV_COLUMNS,
  PATTERN_POV_TEETH,
  PATTERN_POV_SPIRALS,
  PATTERN_POV_KENTE,
  PATTERN_POV_MUDCLOTH,
  PATTERN_POV_SHIELD,
  PATTERN_POV_TATTOO,
  PATTERN_POV_CROSSFIRE,
  PATTERN_POV_HOURGLASS,
  PATTERN_POV_BEADWORK,
  PATTERN_POV_TIPIS,
  PATTERN_POV_RATTLESNAKE,
  PATTERN_POV_STARQUILT,
  PATTERN_POV_AMPHORA,
  PATTERN_POV_PALMETTE,
  PATTERN_POV_BATTLEMENT,
  PATTERN_POV_GUILLOCHE,
  PATTERN_POV_SHARKTEETH,
  PATTERN_POV_SUN,
  PATTERN_POV_CHAIN,
  PATTERN_POV_BASKET,
  PATTERN_POV_LAST = PATTERN_POV_BASKET,

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
