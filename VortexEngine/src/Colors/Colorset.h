#ifndef COLORSET_H
#define COLORSET_H

#include <stdbool.h>
#include "ColorTypes.h"

#include "../VortexConfig.h"

typedef struct ByteStream ByteStream;
typedef struct Random Random;

typedef struct Colorset {
  // palette of colors
  RGBColor m_palette[MAX_COLOR_SLOTS];
  // the current index, starts at UINT8_MAX so that
  // the very first call to getNext will iterate to 0
  uint8_t m_curIndex;
  // the actual number of colors in the set
  uint8_t m_numColors;
} Colorset;

// empty colorset
void Colorset_make(Colorset *self);
// constructor for 1-8 color slots (missing slots default to RGB_OFF)
void Colorset_makeRGB(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4,
  RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);
void Colorset_makeCols(Colorset *self, uint8_t numCols, const uint32_t *cols);
void Colorset_destroy(Colorset *self);

// pads the color list out to the full 8 slots with RGB_OFF
#define Colorset_make1(...) Colorset_makeRGB(__VA_ARGS__, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF)
#define Colorset_make2(...) Colorset_makeRGB(__VA_ARGS__, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF)

// move constructor
void Colorset_moveFrom(Colorset *self, Colorset *other);

// copy and assignment operators
void Colorset_set(Colorset *self, const Colorset *other);

// equality operators
bool Colorset_equal(const Colorset *self, const Colorset *other);
bool Colorset_notEqual(const Colorset *self, const Colorset *other);

// initialize the colorset
void Colorset_init(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4,
  RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);

// clear the colorset
void Colorset_clear(Colorset *self);

// pointer comparison
bool Colorset_equals(const Colorset *self, const Colorset *set);
bool Colorset_equalsPtr(const Colorset *self, const Colorset *set);

// index operator to access color index
RGBColor Colorset_at(const Colorset *self, int index);

// value styles for randomizing colors
typedef uint8_t ColorsetValueStyle;
enum {
  // Random values
  VAL_STYLE_RANDOM = 0,
  // First color low value, the rest are random
  VAL_STYLE_LOW_FIRST_COLOR,
  // First color high value, the rest are low
  VAL_STYLE_HIGH_FIRST_COLOR,
  // Alternat between high and low value
  VAL_STYLE_ALTERNATING,
  // Ascending values from low to high
  VAL_STYLE_ASCENDING,
  // Descending values from high to low
  VAL_STYLE_DESCENDING,
  // Constant value
  VAL_STYLE_CONSTANT,
  // Total number of value styles
  VAL_STYLE_COUNT
};

// add a single color
bool Colorset_addColor(Colorset *self, RGBColor col);
bool Colorset_addColorHSV(Colorset *self, uint8_t hue, uint8_t sat, uint8_t val);
void Colorset_addColorWithValueStyle(Colorset *self, Random *ctx, uint8_t hue, uint8_t sat,
  ColorsetValueStyle valStyle, uint8_t numColors, uint8_t colorPos);
void Colorset_removeColor(Colorset *self, uint8_t index);

// randomization modes for randomizeColors()
typedef uint8_t ColorsetColorMode;
enum {
  THEORY,
  MONOCHROMATIC,
  EVENLY_SPACED
};

// randomize a colorset with a specific number of colors with
// various different randomization techniques
void Colorset_randomize(Colorset *self, Random *ctx, uint8_t numColors);

// function to randomize the colors with various different modes of randomization
void Colorset_randomizeColors(Colorset *self, Random *ctx, uint8_t numColors, ColorsetColorMode mode);

// similar function but with some different modes
typedef uint8_t ColorsetColorMode2;
enum {
  DOUBLE_SPLIT_COMPLIMENTARY,
  TETRADIC
};

void Colorset_randomizeColors2(Colorset *self, Random *ctx, ColorsetColorMode2 mode);

// wrappers for various spacings
void Colorset_randomizeSolid(Colorset *self, Random *ctx);
void Colorset_randomizeComplimentary(Colorset *self, Random *ctx);
void Colorset_randomizeTriadic(Colorset *self, Random *ctx);
void Colorset_randomizeSquare(Colorset *self, Random *ctx);
void Colorset_randomizePentadic(Colorset *self, Random *ctx);
void Colorset_randomizeRainbow(Colorset *self, Random *ctx);

// fade all of the colors in the set
void Colorset_adjustBrightness(Colorset *self, uint8_t fadeby);

// get a color from the colorset
RGBColor Colorset_get(const Colorset *self, uint8_t index);

// set an rgb color in a slot, or add a new color if you specify
// a slot higher than the number of colors in the colorset
void Colorset_setColor(Colorset *self, uint8_t index, RGBColor col);

// skip some amount of colors
void Colorset_skip(Colorset *self, int32_t amount);

// get current color in cycle
RGBColor Colorset_cur(Colorset *self);

// set the current index of the colorset
void Colorset_setCurIndex(Colorset *self, uint8_t index);
void Colorset_resetIndex(Colorset *self);

// the current index
static inline uint8_t Colorset_curIndex(const Colorset *self)
{
  return self->m_curIndex;
}

// get the prev color in cycle
RGBColor Colorset_getPrev(Colorset *self);

// get the next color in cycle
RGBColor Colorset_getNext(Colorset *self);

// peek at the color indexes from current but don't iterate
RGBColor Colorset_peek(const Colorset *self, int32_t offset);

// better wording for peek 1 ahead
static inline RGBColor Colorset_peekNext(const Colorset *self)
{
  return Colorset_peek(self, 1);
}

// the number of colors in the palette
static inline uint8_t Colorset_numColors(const Colorset *self)
{
  return self->m_numColors;
}

// whether the colorset is currently on the first color or last color
bool Colorset_onStart(const Colorset *self);
bool Colorset_onEnd(const Colorset *self);

// serialize the colorset to save/load
bool Colorset_serialize(const Colorset *self, ByteStream *buffer);
bool Colorset_unserialize(Colorset *self, ByteStream *buffer);

// pre-allocate the palette
bool Colorset_initPalette(Colorset *self, uint8_t numColors);

#endif
