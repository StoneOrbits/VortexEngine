#include "Colorset.h"
#include "../Serial/ByteStream.h"
#include "../Random/Random.h"
#include "../Memory/Memory.h"
#include "../Log/Log.h"
#include <string.h>

#define INDEX_NONE UINT8_MAX

void Colorset_init(Colorset *self)
{
  memset(self->palette, 0, sizeof(self->palette));
  self->curIndex = INDEX_NONE;
  self->numColors = 0;
}

void Colorset_initColors(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  Colorset_init(self);
  Colorset_initPalette(self, c1, c2, c3, c4, c5, c6, c7, c8);
}

void Colorset_initFromU32s(Colorset *self, uint8_t numCols, const uint32_t *cols)
{
  Colorset_init(self);
  if (numCols > MAX_COLOR_SLOTS) {
    numCols = MAX_COLOR_SLOTS;
  }
  for (uint8_t i = 0; i < numCols; ++i) {
    RGBColor col;
    RGBColor_initFromU32(&col, cols[i]);
    Colorset_addColor(self, col);
  }
}

void Colorset_copy(Colorset *self, const Colorset *other)
{
  Colorset_initPaletteInternal(self, other->numColors);
  for (uint8_t i = 0; i < other->numColors; ++i) {
    self->palette[i] = other->palette[i];
  }
  Colorset_resetIndex(self);
}

void Colorset_destroy(Colorset *self)
{
  Colorset_clear(self);
}

bool Colorset_equals(const Colorset *self, const Colorset *other)
{
  return (self->numColors == other->numColors) &&
         (memcmp(self->palette, other->palette, self->numColors * sizeof(RGBColor)) == 0);
}

bool Colorset_notEquals(const Colorset *self, const Colorset *other)
{
  return !Colorset_equals(self, other);
}

bool Colorset_equalsPtr(const Colorset *self, const Colorset *set)
{
  if (!set) {
    return false;
  }
  return Colorset_equals(self, set);
}

void Colorset_initPalette(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  Colorset_clear(self);
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
  memset(self->palette, 0, sizeof(self->palette));
  self->numColors = 0;
  Colorset_resetIndex(self);
}

bool Colorset_addColor(Colorset *self, RGBColor col)
{
  if (self->numColors >= MAX_COLOR_SLOTS) {
    return false;
  }
  self->palette[self->numColors] = col;
  self->numColors++;
  return true;
}

bool Colorset_addColorHSV(Colorset *self, uint8_t hue, uint8_t sat, uint8_t val)
{
  RGBColor col;
  HSVColor hsv;
  HSVColor_initHSV(&hsv, hue, sat, val);
  RGBColor_assignHSV(&col, &hsv);
  return Colorset_addColor(self, col);
}

void Colorset_addColorWithValueStyle(Colorset *self, Random *ctx, uint8_t hue, uint8_t sat, ValueStyle valStyle, uint8_t numColors, uint8_t colorPos)
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
    if (self->numColors == 0) {
      Colorset_addColorHSV(self, hue, sat, Random_next8(ctx, 0, 86));
    } else {
      Colorset_addColorHSV(self, hue, sat, 85 * Random_next8(ctx, 1, 4));
    }
    break;
  case VAL_STYLE_HIGH_FIRST_COLOR:
    if (self->numColors == 0) {
      Colorset_addColorHSV(self, hue, sat, 255);
    } else {
      Colorset_addColorHSV(self, hue, sat, Random_next8(ctx, 0, 86));
    }
    break;
  case VAL_STYLE_ALTERNATING:
    if (self->numColors % 2 == 0) {
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
  if (index >= self->numColors) {
    return;
  }
  for (uint8_t i = index; i < (self->numColors - 1); ++i) {
    self->palette[i] = self->palette[i + 1];
  }
  self->numColors--;
  RGBColor_clear(&self->palette[self->numColors]);
}

void Colorset_randomize(Colorset *self, Random *ctx, uint8_t numColors)
{
  Colorset_clear(self);
  if (!numColors) {
    numColors = Random_next8(ctx, 2, 9);
  }
  ValueStyle valStyle = (ValueStyle)Random_next8(ctx, 0, VAL_STYLE_COUNT);

  for (uint8_t i = 0; i < numColors; ++i) {
    uint8_t sat = Random_next8(ctx, 0, 255);
    uint8_t hue = Random_next8(ctx, 0, 255);
    Colorset_addColorWithValueStyle(self, ctx, hue, sat, valStyle, numColors, i);
  }
}

void Colorset_randomizeColors(Colorset *self, Random *ctx, uint8_t numColors, ColorMode mode)
{
  Colorset_clear(self);
  if (!numColors) {
    numColors = Random_next8(ctx, (mode == COLOR_MODE_MONOCHROMATIC) ? 2 : 1, 9);
  }
  uint8_t randomizedHue = Random_next8(ctx, 0, 255);
  uint8_t colorGap = 0;
  if (mode == COLOR_MODE_THEORY && numColors > 1) {
    colorGap = Random_next8(ctx, 16, 256 / (numColors - 1));
  }
  ValueStyle valStyle = (ValueStyle)Random_next8(ctx, 0, VAL_STYLE_COUNT);
  uint8_t doubleStyle = 0;
  if (numColors <= 7) {
    doubleStyle = Random_next8(ctx, 0, 1);
  }
  if (numColors <= 4) {
    doubleStyle = Random_next8(ctx, 0, 2);
  }
  for (uint8_t i = 0; i < numColors; i++) {
    uint8_t hueToUse;
    uint8_t valueToUse = 255;
    if (mode == COLOR_MODE_THEORY) {
      hueToUse = (randomizedHue + (i * colorGap));
    } else if (mode == COLOR_MODE_MONOCHROMATIC) {
      hueToUse = randomizedHue;
      valueToUse = 255 - (i * (256 / numColors));
    } else {
      hueToUse = (randomizedHue + (256 / numColors) * i);
    }
    Colorset_addColorWithValueStyle(self, ctx, hueToUse, valueToUse, valStyle, numColors, i);
    if (doubleStyle == 2 || (doubleStyle == 1 && !i)) {
      Colorset_addColorWithValueStyle(self, ctx, hueToUse, valueToUse, valStyle, numColors, i);
    }
  }
}

void Colorset_randomizeColors2(Colorset *self, Random *ctx, ColorMode2 mode)
{
  Colorset_clear(self);
  uint8_t primaryHue = Random_next8(ctx, 0, 255);
  if (mode == COLOR_MODE2_DOUBLE_SPLIT_COMPLIMENTARY) {
    uint8_t splitGap = Random_next8(ctx, 1, 64);
    ValueStyle valStyle = (ValueStyle)Random_next8(ctx, 0, VAL_STYLE_COUNT);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue + splitGap + 128), 255, valStyle, 5, 0);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue - splitGap), 255, valStyle, 5, 1);
    Colorset_addColorWithValueStyle(self, ctx, primaryHue, 255, valStyle, 5, 2);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue + splitGap), 255, valStyle, 5, 3);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue - splitGap + 128), 255, valStyle, 5, 4);
  } else if (mode == COLOR_MODE2_TETRADIC) {
    uint8_t secondaryHue = Random_next8(ctx, 0, 255);
    ValueStyle valStyle = (ValueStyle)Random_next8(ctx, 0, VAL_STYLE_COUNT);
    Colorset_addColorWithValueStyle(self, ctx, primaryHue, 255, valStyle, 4, 0);
    Colorset_addColorWithValueStyle(self, ctx, secondaryHue, 255, valStyle, 4, 1);
    Colorset_addColorWithValueStyle(self, ctx, (primaryHue + 128), 255, valStyle, 4, 2);
    Colorset_addColorWithValueStyle(self, ctx, (secondaryHue + 128), 255, valStyle, 4, 3);
  }
}

void Colorset_randomizeSolid(Colorset *self, Random *ctx)
{
  Colorset_randomizeColors(self, ctx, 1, COLOR_MODE_EVENLY_SPACED);
}

void Colorset_randomizeComplimentary(Colorset *self, Random *ctx)
{
  Colorset_randomizeColors(self, ctx, 2, COLOR_MODE_EVENLY_SPACED);
}

void Colorset_randomizeTriadic(Colorset *self, Random *ctx)
{
  Colorset_randomizeColors(self, ctx, 3, COLOR_MODE_EVENLY_SPACED);
}

void Colorset_randomizeSquare(Colorset *self, Random *ctx)
{
  Colorset_randomizeColors(self, ctx, 4, COLOR_MODE_EVENLY_SPACED);
}

void Colorset_randomizePentadic(Colorset *self, Random *ctx)
{
  Colorset_randomizeColors(self, ctx, 5, COLOR_MODE_EVENLY_SPACED);
}

void Colorset_randomizeRainbow(Colorset *self, Random *ctx)
{
  Colorset_randomizeColors(self, ctx, 8, COLOR_MODE_EVENLY_SPACED);
}

void Colorset_adjustBrightness(Colorset *self, uint8_t fadeby)
{
  for (uint8_t i = 0; i < self->numColors; ++i) {
    RGBColor_adjustBrightness(&self->palette[i], fadeby);
  }
}

RGBColor Colorset_get(const Colorset *self, uint8_t index)
{
  RGBColor result;
  if (index >= self->numColors) {
    RGBColor_initRGB(&result, 0, 0, 0);
    return result;
  }
  return self->palette[index];
}

void Colorset_set(Colorset *self, uint8_t index, RGBColor col)
{
  if (index >= self->numColors) {
    if (!Colorset_addColor(self, col)) {
      ERROR_LOGF("Failed to add new color at index %u", index);
    }
    return;
  }
  self->palette[index] = col;
}

void Colorset_skip(Colorset *self, int32_t amount)
{
  if (!self->numColors) {
    return;
  }
  if (self->curIndex == INDEX_NONE) {
    self->curIndex = 0;
  }
  amount %= (int32_t)self->numColors;
  self->curIndex = ((int32_t)self->curIndex + (int32_t)amount) % (int32_t)self->numColors;
  if (self->curIndex > self->numColors) {
    self->curIndex += self->numColors;
  }
}

RGBColor Colorset_cur(Colorset *self)
{
  RGBColor result;
  if (self->curIndex >= self->numColors) {
    RGBColor_initRGB(&result, 0, 0, 0);
    return result;
  }
  if (self->curIndex == INDEX_NONE) {
    return self->palette[0];
  }
  return self->palette[self->curIndex];
}

void Colorset_setCurIndex(Colorset *self, uint8_t index)
{
  if (!self->numColors) {
    return;
  }
  if (index > (self->numColors - 1)) {
    return;
  }
  self->curIndex = index;
}

void Colorset_resetIndex(Colorset *self)
{
  self->curIndex = INDEX_NONE;
}

uint8_t Colorset_curIndex(const Colorset *self)
{
  return self->curIndex;
}

RGBColor Colorset_getPrev(Colorset *self)
{
  RGBColor result;
  if (!self->numColors) {
    RGBColor_initFromU32(&result, 0);
    return result;
  }
  if (self->curIndex == 0 || self->curIndex == INDEX_NONE) {
    self->curIndex = Colorset_numColors(self) - 1;
  } else {
    self->curIndex--;
  }
  return self->palette[self->curIndex];
}

RGBColor Colorset_getNext(Colorset *self)
{
  RGBColor result;
  if (!self->numColors) {
    RGBColor_initFromU32(&result, 0);
    return result;
  }
  self->curIndex++;
  self->curIndex %= Colorset_numColors(self);
  return self->palette[self->curIndex];
}

RGBColor Colorset_peek(const Colorset *self, int32_t offset)
{
  RGBColor result;
  if (!self->numColors) {
    RGBColor_initFromU32(&result, 0);
    return result;
  }
  uint8_t nextIndex = 0;
  if (offset >= 0) {
    nextIndex = (self->curIndex + offset) % Colorset_numColors(self);
  } else {
    if (offset < -1 * (int32_t)(Colorset_numColors(self))) {
      RGBColor_initFromU32(&result, 0);
      return result;
    }
    nextIndex = ((self->curIndex + Colorset_numColors(self)) + (int)offset) % Colorset_numColors(self);
  }
  return self->palette[nextIndex];
}

RGBColor Colorset_peekNext(const Colorset *self)
{
  return Colorset_peek(self, 1);
}

uint8_t Colorset_numColors(const Colorset *self)
{
  return self->numColors;
}

bool Colorset_onStart(const Colorset *self)
{
  return (self->curIndex == 0);
}

bool Colorset_onEnd(const Colorset *self)
{
  if (!self->numColors) {
    return false;
  }
  return (self->curIndex == self->numColors - 1);
}

bool Colorset_serialize(const Colorset *self, ByteStream *buffer)
{
  if (!ByteStream_serialize8(buffer, self->numColors)) {
    return false;
  }
  for (uint8_t i = 0; i < self->numColors; ++i) {
    if (!ByteStream_serialize8(buffer, self->palette[i].red)) {
      return false;
    }
  }
  for (uint8_t i = 0; i < self->numColors; ++i) {
    if (!ByteStream_serialize8(buffer, self->palette[i].green)) {
      return false;
    }
  }
  for (uint8_t i = 0; i < self->numColors; ++i) {
    if (!ByteStream_serialize8(buffer, self->palette[i].blue)) {
      return false;
    }
  }
  return true;
}

bool Colorset_unserialize(Colorset *self, ByteStream *buffer)
{
  if (!ByteStream_unserialize8(buffer, &self->numColors)) {
    return false;
  }
  if (self->numColors > MAX_COLOR_SLOTS) {
    return false;
  }
  if (!Colorset_initPaletteInternal(self, self->numColors)) {
    return false;
  }
  for (uint8_t i = 0; i < self->numColors; ++i) {
    if (!ByteStream_unserialize8(buffer, &self->palette[i].red)) {
      return false;
    }
  }
  for (uint8_t i = 0; i < self->numColors; ++i) {
    if (!ByteStream_unserialize8(buffer, &self->palette[i].green)) {
      return false;
    }
  }
  for (uint8_t i = 0; i < self->numColors; ++i) {
    if (!ByteStream_unserialize8(buffer, &self->palette[i].blue)) {
      return false;
    }
  }
  return true;
}

bool Colorset_initPaletteInternal(Colorset *self, uint8_t numColors)
{
  Colorset_clear(self);
  self->numColors = numColors;
  return true;
}
