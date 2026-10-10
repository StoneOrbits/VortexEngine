#include "Colorset.h"

#include "../Serial/ByteStream.h"
#include "../Random/Random.h"
#include "../Memory/Memory.h"

#include "../Log/Log.h"

#include <string.h>

// when no color is selected in the colorset the index is this
// then when you call getNext() for the first time it returns
// the 0th color in the colorset and after the index will be 0
#define INDEX_NONE UINT8_MAX

void Colorset_make(Colorset *self)
{
  memset((void *)self->m_palette, 0, sizeof(self->m_palette));
  self->m_curIndex = INDEX_NONE;
  self->m_numColors = 0;
  Colorset_init(self, RGBColor_make(), RGBColor_make(), RGBColor_make(), RGBColor_make(),
    RGBColor_make(), RGBColor_make(), RGBColor_make(), RGBColor_make());
}

// constructor for 1-8 color slots
void Colorset_makeRGB(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4,
  RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  Colorset_make(self);
  Colorset_init(self, c1, c2, c3, c4, c5, c6, c7, c8);
}

void Colorset_makeCols(Colorset *self, uint8_t numCols, const uint32_t *cols)
{
  Colorset_make(self);
  if (numCols > MAX_COLOR_SLOTS) {
    numCols = MAX_COLOR_SLOTS;
  }
  for (uint8_t i = 0; i < numCols; ++i) {
    Colorset_addColor(self, RGBColor_fromU32(cols[i]));
  }
}

void Colorset_destroy(Colorset *self)
{
  Colorset_clear(self);
}

// move constructor
void Colorset_moveFrom(Colorset *self, Colorset *other)
{
  memset((void *)self->m_palette, 0, sizeof(self->m_palette));
  self->m_curIndex = INDEX_NONE;
  self->m_numColors = other->m_numColors;
  memcpy((void *)self->m_palette, (void *)other->m_palette, sizeof(self->m_palette));
  memset((void *)other->m_palette, 0, sizeof(other->m_palette));
  other->m_numColors = 0;
  other->m_curIndex = INDEX_NONE;
}

// copy and assignment operators
void Colorset_set(Colorset *self, const Colorset *other)
{
  Colorset_initPalette(self, other->m_numColors);
  for (uint8_t i = 0; i < other->m_numColors; ++i) {
    self->m_palette[i] = other->m_palette[i];
  }
  Colorset_resetIndex(self);
}

// equality operators
bool Colorset_equal(const Colorset *self, const Colorset *other)
{
  // only compare the palettes for equality
  return (self->m_numColors == other->m_numColors) &&
         (memcmp(self->m_palette, other->m_palette, self->m_numColors * sizeof(RGBColor)) == 0);
}

bool Colorset_notEqual(const Colorset *self, const Colorset *other)
{
  return !Colorset_equal(self, other);
}

void Colorset_init(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4,
  RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  // clear any existing colors
  Colorset_clear(self);
  // would be nice if we could do this another way
  if (!RGBColor_empty(&c1)) Colorset_addColor(self, c1);
  if (!RGBColor_empty(&c2)) Colorset_addColor(self, c2);
  if (!RGBColor_empty(&c3)) Colorset_addColor(self, c3);
  if (!RGBColor_empty(&c4)) Colorset_addColor(self, c4);
  if (!RGBColor_empty(&c5)) Colorset_addColor(self, c5);
  if (!RGBColor_empty(&c6)) Colorset_addColor(self, c6);
  if (!RGBColor_empty(&c7)) Colorset_addColor(self, c7);
  if (!RGBColor_empty(&c8)) Colorset_addColor(self, c8);
}

void Colorset_clear(Colorset *self)
{
  memset((void *)self->m_palette, 0, sizeof(self->m_palette));
  self->m_numColors = 0;
  Colorset_resetIndex(self);
}

bool Colorset_equals(const Colorset *self, const Colorset *set)
{
  return Colorset_equal(self, set);
}

bool Colorset_equalsPtr(const Colorset *self, const Colorset *set)
{
  if (!set) {
    return false;
  }
  return Colorset_equal(self, set);
}

RGBColor Colorset_at(const Colorset *self, int index)
{
  return Colorset_get(self, index);
}

// add a single color
bool Colorset_addColor(Colorset *self, RGBColor col)
{
  if (self->m_numColors >= MAX_COLOR_SLOTS) {
    return false;
  }
  // insert new color and increment number of colors
  self->m_palette[self->m_numColors] = col;
  self->m_numColors++;
  return true;
}

bool Colorset_addColorHSV(Colorset *self, uint8_t hue, uint8_t sat, uint8_t val)
{
  HSVColor hsv = HSVColor_makeHSV(hue, sat, val);
  return Colorset_addColor(self, RGBColor_fromHSV(&hsv));
}

void Colorset_addColorWithValueStyle(Colorset *self, Random *ctx, uint8_t hue, uint8_t sat, ColorsetValueStyle valStyle, uint8_t numColors, uint8_t colorPos)
{
  if (numColors == 1) {
    Colorset_addColorHSV(self, hue, sat, Random_next8(ctx, 16, 255));
    return;
  }
  switch (valStyle) {
  default:
  case VAL_STYLE_RANDOM:
    Colorset_addColorHSV(self, hue, sat, 85 * Random_next8(ctx, 1, 4));
    break;
  case VAL_STYLE_LOW_FIRST_COLOR:
    if (self->m_numColors == 0) {
      Colorset_addColorHSV(self, hue, sat, Random_next8(ctx, 0, 86));
    } else {
      Colorset_addColorHSV(self, hue, sat, 85 * Random_next8(ctx, 1, 4));
    }
    break;
  case VAL_STYLE_HIGH_FIRST_COLOR:
    if (self->m_numColors == 0) {
      Colorset_addColorHSV(self, hue, sat, 255);
    } else {
      Colorset_addColorHSV(self, hue, sat, Random_next8(ctx, 0, 86));
    }
    break;
  case VAL_STYLE_ALTERNATING:
    if (self->m_numColors % 2 == 0) {
      Colorset_addColorHSV(self, hue, sat, 255);
    } else {
      Colorset_addColorHSV(self, hue, sat, 85);
    }
    break;
  case VAL_STYLE_ASCENDING:
    Colorset_addColorHSV(self, hue, sat, (colorPos + 1) * (255 / numColors));
    break;
  case VAL_STYLE_DESCENDING:
    Colorset_addColorHSV(self, hue, sat, 255 - (colorPos * (255 / numColors)));
    break;
  case VAL_STYLE_CONSTANT:
    Colorset_addColorHSV(self, hue, sat, 255);
  }
}

void Colorset_removeColor(Colorset *self, uint8_t index)
{
  if (index >= self->m_numColors) {
    return;
  }
  for (uint8_t i = index; i < (self->m_numColors - 1); ++i) {
    self->m_palette[i] = self->m_palette[i + 1];
  }
  RGBColor_clear(&self->m_palette[--self->m_numColors]);
}

// create a set of truely random colors
void Colorset_randomize(Colorset *self, Random *ctx, uint8_t numColors)
{
  Colorset_clear(self);
  if (!numColors) {
    numColors = Random_next8(ctx, 2, 9);
  }
  ColorsetValueStyle valStyle = (ColorsetValueStyle)Random_next8(ctx, 0, VAL_STYLE_COUNT);

  for (uint8_t i = 0; i < numColors; ++i) {
    // call next8 explicitly in this order because the order they
    // are called is undefined when called as parameters to another function.
    // ex: f(a,b,c) may call in the order a,b,c or c,b,a depending on compiler.
    // So different compilers may produce different results, 
    // but like this it is explicit
    uint8_t sat = Random_next8Default(ctx);
    uint8_t hue = Random_next8Default(ctx);
    Colorset_addColorWithValueStyle(self, ctx, hue, sat, valStyle, numColors, i);
  }
}

void Colorset_randomizeColors(Colorset *self, Random *ctx, uint8_t numColors, ColorsetColorMode mode)
{
  Colorset_clear(self);
  if (!numColors) {
    numColors = Random_next8(ctx, mode == MONOCHROMATIC ? 2 : 1, 9);
  }
  uint8_t randomizedHue = Random_next8Default(ctx);
  uint8_t colorGap = 0;
  if (mode == THEORY && numColors > 1) {
    colorGap = Random_next8(ctx, 16, 256 / (numColors - 1));
  }
  ColorsetValueStyle valStyle = (ColorsetValueStyle)Random_next8(ctx, 0, VAL_STYLE_COUNT);
  // the doubleStyle decides if some colors are added to the set twice
  uint8_t doubleStyle = 0;
  if (numColors <= 7) {
    doubleStyle = (Random_next8(ctx, 0, 1));
  }
  if (numColors <= 4) {
    doubleStyle = (Random_next8(ctx, 0, 2));
  }
  for (uint8_t i = 0; i < numColors; i++) {
    uint8_t hueToUse;
    uint8_t valueToUse = 255;
    if (mode == THEORY) {
      hueToUse = (randomizedHue + (i * colorGap));
    } else if (mode == MONOCHROMATIC) {
      hueToUse = randomizedHue;
      valueToUse = 255 - (i * (256 / numColors));
    } else { // EVENLY_SPACED
      hueToUse = (randomizedHue + (256 / numColors) * i);
    }
    Colorset_addColorWithValueStyle(self, ctx, hueToUse, valueToUse, valStyle, numColors, i);
    // double all colors or only first color
    if (doubleStyle == 2 || (doubleStyle == 1 && !i)) {
      Colorset_addColorWithValueStyle(self, ctx, hueToUse, valueToUse, valStyle, numColors, i);
    }
  }
}

void Colorset_randomizeColors2(Colorset *self, Random *ctx, ColorsetColorMode2 mode)
{
  Colorset_clear(self);
  uint8_t primaryHue = Random_next8Default(ctx);
  if (mode == DOUBLE_SPLIT_COMPLIMENTARY) {
    uint8_t splitGap = Random_next8(ctx, 1, 64);
    ColorsetValueStyle valStyle = (ColorsetValueStyle)Random_next8(ctx, 0, VAL_STYLE_COUNT);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue + splitGap + 128), 255, valStyle, 5, 0);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue - splitGap), 255, valStyle, 5, 1);
    Colorset_addColorWithValueStyle(self, ctx, primaryHue, 255, valStyle, 5, 2);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue + splitGap), 255, valStyle, 5, 3);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue - splitGap + 128), 255, valStyle, 5, 4);
  } else if (mode == TETRADIC) {
    uint8_t secondaryHue = Random_next8Default(ctx);
    ColorsetValueStyle valStyle = (ColorsetValueStyle)Random_next8(ctx, 0, VAL_STYLE_COUNT);
    Colorset_addColorWithValueStyle(self, ctx, primaryHue, 255, valStyle, 4, 0);
    Colorset_addColorWithValueStyle(self, ctx, secondaryHue, 255, valStyle, 4, 1);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue + 128), 255, valStyle, 4, 2);
    Colorset_addColorWithValueStyle(self, ctx, (secondaryHue + 128), 255, valStyle, 4, 3);
  }
}

void Colorset_adjustBrightness(Colorset *self, uint8_t fadeby)
{
  for (uint8_t i = 0; i < self->m_numColors; ++i) {
    RGBColor_adjustBrightness(&self->m_palette[i], fadeby);
  }
}

// get a color from the colorset
RGBColor Colorset_get(const Colorset *self, uint8_t index)
{
  if (index >= self->m_numColors) {
    return RGBColor_make();
  }
  return self->m_palette[index];
}

// set an rgb color in a slot, or add a new color if you specify
// a slot higher than the number of colors in the colorset
void Colorset_setColor(Colorset *self, uint8_t index, RGBColor col)
{
  // special case for 'setting' a color at the edge of the palette,
  // ie adding a new color when you set an index higher than the max
  if (index >= self->m_numColors) {
    if (!Colorset_addColor(self, col)) {
      ERROR_LOGF("Failed to add new color at index %u", index);
    }
    return;
  }
  self->m_palette[index] = col;
}

// skip some amount of colors
void Colorset_skip(Colorset *self, int32_t amount)
{
  if (!self->m_numColors) {
    return;
  }
  // if the colorset hasn't started yet
  if (self->m_curIndex == INDEX_NONE) {
    self->m_curIndex = 0;
  }

  // first modulate the amount to skip to be within +/- the number of colors
  amount %= (int32_t)self->m_numColors;

  // max = 3
  // self->m_curIndex = 2
  // amount = -10
  self->m_curIndex = ((int32_t)self->m_curIndex + (int32_t)amount) % (int32_t)self->m_numColors;
  if (self->m_curIndex > self->m_numColors) { // must have wrapped
    // simply wrap it back
    self->m_curIndex += self->m_numColors;
  }
}

RGBColor Colorset_cur(Colorset *self)
{
  if (self->m_curIndex >= self->m_numColors) {
    return RGBColor_make();
  }
  if (self->m_curIndex == INDEX_NONE) {
    return self->m_palette[0];
  }
  return self->m_palette[self->m_curIndex];
}

void Colorset_setCurIndex(Colorset *self, uint8_t index)
{
  if (!self->m_numColors) {
    return;
  }
  if (index > (self->m_numColors - 1)) {
    return;
  }
  self->m_curIndex = index;
}

void Colorset_resetIndex(Colorset *self)
{
  self->m_curIndex = INDEX_NONE;
}

RGBColor Colorset_getPrev(Colorset *self)
{
  if (!self->m_numColors) {
    return RGBColor_make();
  }
  // handle wrapping at 0
  if (self->m_curIndex == 0 || self->m_curIndex == INDEX_NONE) {
    self->m_curIndex = Colorset_numColors(self) - 1;
  } else {
    self->m_curIndex--;
  }
  // return the color
  return self->m_palette[self->m_curIndex];
}

RGBColor Colorset_getNext(Colorset *self)
{
  if (!self->m_numColors) {
    return RGBColor_make();
  }
  // iterate current index, let it wrap at max uint8
  self->m_curIndex++;
  // then modulate the result within max colors
  self->m_curIndex %= Colorset_numColors(self);
  // return the color
  return self->m_palette[self->m_curIndex];
}

// peek at the next color but don't iterate
RGBColor Colorset_peek(const Colorset *self, int32_t offset)
{
  if (!self->m_numColors) {
    return RGBColor_make();
  }
  uint8_t nextIndex = 0;
  // get index of the next color
  if (offset >= 0) {
    nextIndex = (self->m_curIndex + offset) % Colorset_numColors(self);
  } else {
    if (offset < -1 * (int32_t)(Colorset_numColors(self))) {
      return RGBColor_make();
    }
    nextIndex = ((self->m_curIndex + Colorset_numColors(self)) + (int)offset) % Colorset_numColors(self);
  }
  // return the color
  return self->m_palette[nextIndex];
}

bool Colorset_onStart(const Colorset *self)
{
  return (self->m_curIndex == 0);
}

bool Colorset_onEnd(const Colorset *self)
{
  if (!self->m_numColors) {
    return false;
  }
  return (self->m_curIndex == self->m_numColors - 1);
}

bool Colorset_serialize(const Colorset *self, ByteStream *buffer)
{
  if (!ByteStream_serialize8(buffer, self->m_numColors)) {
    return false;
  }
  // write all the reds/greens/blues together to maximize chance of
  // repeated values to improve RLE compression
  for (uint8_t i = 0; i < self->m_numColors; ++i) {
    if (!ByteStream_serialize8(buffer, self->m_palette[i].red)) {
      return false;
    }
  }
  for (uint8_t i = 0; i < self->m_numColors; ++i) {
    if (!ByteStream_serialize8(buffer, self->m_palette[i].green)) {
      return false;
    }
  }
  for (uint8_t i = 0; i < self->m_numColors; ++i) {
    if (!ByteStream_serialize8(buffer, self->m_palette[i].blue)) {
      return false;
    }
  }
  return true;
}

bool Colorset_unserialize(Colorset *self, ByteStream *buffer)
{
  if (!ByteStream_unserialize8(buffer, &self->m_numColors)) {
    return false;
  }
  if (self->m_numColors > MAX_COLOR_SLOTS) {
    return false;
  }
  if (!Colorset_initPalette(self, self->m_numColors)) {
    return false;
  }
  for (uint8_t i = 0; i < self->m_numColors; ++i) {
    if (!ByteStream_unserialize8(buffer, &self->m_palette[i].red)) {
      return false;
    }
  }
  for (uint8_t i = 0; i < self->m_numColors; ++i) {
    if (!ByteStream_unserialize8(buffer, &self->m_palette[i].green)) {
      return false;
    }
  }
  for (uint8_t i = 0; i < self->m_numColors; ++i) {
    if (!ByteStream_unserialize8(buffer, &self->m_palette[i].blue)) {
      return false;
    }
  }
  return true;
}

bool Colorset_initPalette(Colorset *self, uint8_t numColors)
{
  Colorset_clear(self);
  self->m_numColors = numColors;
  return true;
}

