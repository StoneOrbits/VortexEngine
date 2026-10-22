#ifndef COLOR_H
#define COLOR_H

#include <inttypes.h>
#include <stdbool.h>

#include "../VortexConfig.h"
#include "ColorConstants.h"

// kept as a byte so the global selector costs no extra ram
typedef uint8_t hsv_to_rgb_algorithm;
enum {
  HSV_TO_RGB_GENERIC,
  HSV_TO_RGB_RAW,
  HSV_TO_RGB_RAINBOW
};

// global hsv to rgb algorithm selector, switch this to control
// all hsv to rgb conversions
extern hsv_to_rgb_algorithm g_hsv_rgb_alg;

typedef struct ByteStream ByteStream;
typedef struct RGBColor RGBColor;
typedef struct HSVColor HSVColor;

typedef struct HSVColor {
  // public members
  uint8_t hue;
  uint8_t sat;
  uint8_t val;
} HSVColor;

// construction
HSVColor HSVColor_make(void);
HSVColor HSVColor_makeHSV(uint8_t hue, uint8_t sat, uint8_t val);
// assignment from uint32_t
HSVColor HSVColor_fromU32(uint32_t dwVal);
void HSVColor_setU32(HSVColor *self, uint32_t rhs);
// copy/assignment construction
void HSVColor_setHSV(HSVColor *self, const HSVColor *rhs);
// construction/assignment from RGB
void HSVColor_setRGB(HSVColor *self, const RGBColor *rhs);
// equality operators
bool HSVColor_equals(const HSVColor *self, const HSVColor *other);

bool HSVColor_empty(const HSVColor *self);
void HSVColor_clear(HSVColor *self);

static inline uint32_t HSVColor_raw(const HSVColor *self)
{
  return HSV_BIT | ((uint32_t)self->hue << 16) | ((uint32_t)self->sat << 8) | (uint32_t)self->val;
}

typedef struct RGBColor {
  // public members
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} RGBColor;

// construction
RGBColor RGBColor_make(void);
RGBColor RGBColor_makeRGB(uint8_t red, uint8_t green, uint8_t blue);
// assignment from uint32_t
RGBColor RGBColor_fromU32(uint32_t dwVal);
void RGBColor_setU32(RGBColor *self, uint32_t rhs);
// copy/assignment construction
void RGBColor_setRGB(RGBColor *self, const RGBColor *rhs);
// construction/assignment from HSV
RGBColor RGBColor_fromHSV(const HSVColor *rhs);
void RGBColor_setHSV(RGBColor *self, const HSVColor *rhs);
// equality operators
bool RGBColor_equals(const RGBColor *self, const RGBColor *other);

bool RGBColor_empty(const RGBColor *self);
void RGBColor_clear(RGBColor *self);

void RGBColor_adjustBrightness(RGBColor *self, uint8_t fadeBy);
bool RGBColor_serialize(const RGBColor *self, ByteStream *buffer);
bool RGBColor_unserialize(RGBColor *self, ByteStream *buffer);

static inline uint32_t RGBColor_raw(const RGBColor *self)
{
  return ((uint32_t)self->red << 16) | ((uint32_t)self->green << 8) | (uint32_t)self->blue;
}

// Stolen from FastLED hsv to rgb full rainbox where all colours
// are given equal weight, this makes for-example yellow larger
// best to use this function as it is the legacy choice
RGBColor hsv_to_rgb_rainbow(const HSVColor *rhs);
// stolen from FastLED raw hsv to rgb conversion, idk what is special
// about this one but I think the hue stops at 190 maybe?
RGBColor hsv_to_rgb_raw_C(const HSVColor *rhs);
// generic hsv to rgb conversion nothing special
RGBColor hsv_to_rgb_generic(const HSVColor *rhs);

// Convert rgb to hsv with slower approximation that matches
// the fastled hsv to rgb 'rainbow'
HSVColor rgb_to_hsv_approx(const RGBColor *rhs);
// Convert rgb to hsv with generic fast method
HSVColor rgb_to_hsv_generic(const RGBColor *rhs);

#endif
