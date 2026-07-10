#ifndef COLOR_H
#define COLOR_H

#include <inttypes.h>
#include <stdbool.h>

#include "../VortexConfig.h"
#include "ColorConstants.h"

typedef uint8_t hsv_to_rgb_algorithm;

enum {
  HSV_TO_RGB_GENERIC,
  HSV_TO_RGB_RAW,
  HSV_TO_RGB_RAINBOW
};

extern hsv_to_rgb_algorithm g_hsv_rgb_alg;

typedef struct ByteStream ByteStream;

typedef struct RGBColor_t RGBColor;
typedef struct HSVColor_t HSVColor;

struct HSVColor_t {
  uint8_t hue;
  uint8_t sat;
  uint8_t val;
};

struct RGBColor_t {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

void HSVColor_init(HSVColor *self);
void HSVColor_initHSV(HSVColor *self, uint8_t hue, uint8_t sat, uint8_t val);
void HSVColor_initFromU32(HSVColor *self, uint32_t dwVal);
void HSVColor_copy(HSVColor *self, const HSVColor *rhs);
void HSVColor_initFromRGB(HSVColor *self, const RGBColor *rhs);
void HSVColor_assignRGB(HSVColor *self, const RGBColor *rhs);
void HSVColor_assignU32(HSVColor *self, const uint32_t *rhs);
bool HSVColor_equals(const HSVColor *self, const HSVColor *other);
bool HSVColor_notEquals(const HSVColor *self, const HSVColor *other);
bool HSVColor_empty(const HSVColor *self);
void HSVColor_clear(HSVColor *self);
uint32_t HSVColor_raw(const HSVColor *self);

void RGBColor_init(RGBColor *self);
void RGBColor_initRGB(RGBColor *self, uint8_t red, uint8_t green, uint8_t blue);
void RGBColor_initFromU32(RGBColor *self, uint32_t dwVal);
void RGBColor_copy(RGBColor *self, const RGBColor *rhs);
void RGBColor_initFromHSV(RGBColor *self, const HSVColor *rhs);
void RGBColor_assignU32(RGBColor *self, const uint32_t *rhs);
void RGBColor_assignHSV(RGBColor *self, const HSVColor *rhs);
bool RGBColor_equals(const RGBColor *self, const RGBColor *other);
bool RGBColor_notEquals(const RGBColor *self, const RGBColor *other);
bool RGBColor_empty(const RGBColor *self);
void RGBColor_clear(RGBColor *self);
void RGBColor_adjustBrightness(RGBColor *self, uint8_t fadeBy);
uint32_t RGBColor_raw(const RGBColor *self);

bool RGBColor_serialize(const RGBColor *self, ByteStream *buffer);
bool RGBColor_unserialize(RGBColor *self, ByteStream *buffer);

RGBColor hsv_to_rgb_rainbow(const HSVColor *rhs);
RGBColor hsv_to_rgb_raw_C(const HSVColor *rhs);
RGBColor hsv_to_rgb_generic(const HSVColor *rhs);
HSVColor rgb_to_hsv_approx(const RGBColor *rhs);
HSVColor rgb_to_hsv_generic(const RGBColor *rhs);

// Convert uint32_t color value to RGBColor struct (C has no implicit uint32_t→RGBColor)
#define RGB_COLOR(v) ((RGBColor){ .red = (uint8_t)((v) >> 16), .green = (uint8_t)((v) >> 8), .blue = (uint8_t)(v) })
#define HSV_COLOR(v) ((HSVColor){ .hue = (uint8_t)((v) >> 16), .sat = (uint8_t)((v) >> 8), .val = (uint8_t)(v) })

#endif
