#ifndef COLOR_H
#define COLOR_H

#include <inttypes.h>

#include "../VortexConfig.h"
#include "ColorConstants.h"

enum hsv_to_rgb_algorithm : uint8_t
{
  HSV_TO_RGB_GENERIC,
  HSV_TO_RGB_RAW,
  HSV_TO_RGB_RAINBOW
};

// global hsv to rgb algorithm selector, switch this to control
// all hsv to rgb conversions
extern hsv_to_rgb_algorithm g_hsv_rgb_alg;

class ByteStream;
class RGBColor;

class HSVColor
{
public:
  HSVColor();
  HSVColor(uint8_t hue, uint8_t sat, uint8_t val);

  // assignment from uint32_t
  HSVColor(uint32_t dwVal);
  HSVColor &operator=(const uint32_t &rhs);

  // copy/assignment construction
  HSVColor(const HSVColor &rhs);
  HSVColor &operator=(const HSVColor &rhs);

  // construction/assignment from RGB
  HSVColor(const RGBColor &rhs);
  HSVColor &operator=(const RGBColor &rhs);

  // equality operators
  bool operator==(const HSVColor &other) const;
  bool operator!=(const HSVColor &other) const;

  bool empty() const;
  void clear();

  uint32_t raw() const { return HSV_BIT | ((uint32_t)hue << 16) | ((uint32_t)sat << 8) | (uint32_t)val; }

  // public members
  uint8_t hue;
  uint8_t sat;
  uint8_t val;
};

class RGBColor
{
public:
  RGBColor();
  RGBColor(uint8_t red, uint8_t green, uint8_t blue);

  // assignment from uint32_t
  RGBColor(uint32_t dwVal);
  RGBColor &operator=(const uint32_t &rhs);

  // copy/assignment construction
  RGBColor(const RGBColor &rhs);
  RGBColor &operator=(const RGBColor &rhs);

  // construction/assignment from HSV
  RGBColor(const HSVColor &rhs);
  RGBColor &operator=(const HSVColor &rhs);

  // equality operators
  bool operator==(const RGBColor &other) const;
  bool operator!=(const RGBColor &other) const;

  bool empty() const;
  void clear();

  RGBColor adjustBrightness(uint8_t fadeBy);
  bool serialize(ByteStream &buffer) const;
  bool unserialize(ByteStream &buffer);

  uint32_t raw() const { return ((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue; }

  // public members
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// Stolen from FastLED hsv to rgb full rainbox where all colours
// are given equal weight, this makes for-example yellow larger
// best to use this function as it is the legacy choice
RGBColor hsv_to_rgb_rainbow(const HSVColor &rhs);
// stolen from FastLED raw hsv to rgb conversion, idk what is special
// about this one but I think the hue stops at 190 maybe?
RGBColor hsv_to_rgb_raw_C(const HSVColor &rhs);
// generic hsv to rgb conversion nothing special
RGBColor hsv_to_rgb_generic(const HSVColor &rhs);

// Convert rgb to hsv with slower approximation that matches
// the fastled hsv to rgb 'rainbow'
HSVColor rgb_to_hsv_approx(const RGBColor &rhs);
// Convert rgb to hsv with generic fast method
HSVColor rgb_to_hsv_generic(const RGBColor &rhs);

#endif
