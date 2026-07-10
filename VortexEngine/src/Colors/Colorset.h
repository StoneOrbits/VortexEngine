#ifndef COLORSET_H
#define COLORSET_H

#include "ColorTypes.h"
#include "../VortexConfig.h"
#include <stdbool.h>

typedef struct ByteStream ByteStream;
#include "../Random/Random.h"

typedef uint8_t ValueStyle;
enum {
  VAL_STYLE_RANDOM = 0,
  VAL_STYLE_LOW_FIRST_COLOR,
  VAL_STYLE_HIGH_FIRST_COLOR,
  VAL_STYLE_ALTERNATING,
  VAL_STYLE_ASCENDING,
  VAL_STYLE_DESCENDING,
  VAL_STYLE_CONSTANT,
  VAL_STYLE_COUNT
};

typedef uint8_t ColorMode;
enum {
  COLOR_MODE_THEORY,
  COLOR_MODE_MONOCHROMATIC,
  COLOR_MODE_EVENLY_SPACED
};

typedef uint8_t ColorMode2;
enum {
  COLOR_MODE2_DOUBLE_SPLIT_COMPLIMENTARY,
  COLOR_MODE2_TETRADIC
};

typedef struct Colorset_s {
  RGBColor palette[MAX_COLOR_SLOTS];
  uint8_t curIndex;
  uint8_t numColors;
} Colorset;

void Colorset_init(Colorset *self);
void Colorset_initColors(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);
void Colorset_initFromU32s(Colorset *self, uint8_t numCols, const uint32_t *cols);
void Colorset_copy(Colorset *self, const Colorset *other);
void Colorset_destroy(Colorset *self);
bool Colorset_equals(const Colorset *self, const Colorset *other);
bool Colorset_notEquals(const Colorset *self, const Colorset *other);
bool Colorset_equalsPtr(const Colorset *self, const Colorset *set);
void Colorset_initPalette(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);
void Colorset_clear(Colorset *self);
bool Colorset_addColor(Colorset *self, RGBColor col);
bool Colorset_addColorHSV(Colorset *self, uint8_t hue, uint8_t sat, uint8_t val);
void Colorset_addColorWithValueStyle(Colorset *self, Random *ctx, uint8_t hue, uint8_t sat, ValueStyle valStyle, uint8_t numColors, uint8_t colorPos);
void Colorset_removeColor(Colorset *self, uint8_t index);
void Colorset_randomize(Colorset *self, Random *ctx, uint8_t numColors);
void Colorset_randomizeColors(Colorset *self, Random *ctx, uint8_t numColors, ColorMode mode);
void Colorset_randomizeColors2(Colorset *self, Random *ctx, ColorMode2 mode);
void Colorset_randomizeSolid(Colorset *self, Random *ctx);
void Colorset_randomizeComplimentary(Colorset *self, Random *ctx);
void Colorset_randomizeTriadic(Colorset *self, Random *ctx);
void Colorset_randomizeSquare(Colorset *self, Random *ctx);
void Colorset_randomizePentadic(Colorset *self, Random *ctx);
void Colorset_randomizeRainbow(Colorset *self, Random *ctx);
void Colorset_adjustBrightness(Colorset *self, uint8_t fadeby);
RGBColor Colorset_get(const Colorset *self, uint8_t index);
void Colorset_set(Colorset *self, uint8_t index, RGBColor col);
void Colorset_skip(Colorset *self, int32_t amount);
RGBColor Colorset_cur(Colorset *self);
void Colorset_setCurIndex(Colorset *self, uint8_t index);
void Colorset_resetIndex(Colorset *self);
uint8_t Colorset_curIndex(const Colorset *self);
RGBColor Colorset_getPrev(Colorset *self);
RGBColor Colorset_getNext(Colorset *self);
RGBColor Colorset_peek(const Colorset *self, int32_t offset);
RGBColor Colorset_peekNext(const Colorset *self);
uint8_t Colorset_numColors(const Colorset *self);
bool Colorset_onStart(const Colorset *self);
bool Colorset_onEnd(const Colorset *self);
bool Colorset_serialize(const Colorset *self, ByteStream *buffer);
bool Colorset_unserialize(Colorset *self, ByteStream *buffer);
bool Colorset_initPaletteInternal(Colorset *self, uint8_t numColors);

#endif
