#ifndef COLOR_H
#define COLOR_H

#include <inttypes.h>

// if this bit is present it's an HSV constant
#define HSV_BIT (1 << 31)

// produce a DWORD HSV constant
#define HSV(h, s, v) (HSV_BIT | (h << 16) | (s << 8) | v)

// Pre defined hex HSV values
#define HSV_WHITE   HSV_BIT | 0x00006E    // 0 0 110
#define HSV_ORANGE  HSV_BIT | 0x14FF6E    // 20 255 110
#define HSV_BLUE    HSV_BIT | 0xA0FF6E    // 160 255 110
#define HSV_YELLOW  HSV_BIT | 0x3CFF6E    // 60  255 110
#define HSV_RED     HSV_BIT | 0x00FF6E    // 0   255 110
#define HSV_GREEN   HSV_BIT | 0x55FF6E    // 85  255 110
#define HSV_TEAL    HSV_BIT | 0x78FF6E    // 120 255 110
#define HSV_PURPLE  HSV_BIT | 0xD4FF6E    // 212 255 110
#define HSV_BLANK   HSV_BIT | 0x000040    //   0   0  40
#define HSV_OFF     HSV_BIT | 0x000000    //   0   0   0

#define RGB_WHITE       0xaaaaaa    // 170 170 170
#define RGB_DIM_WHITE   0x787878    // 120 120 120
#define RGB_ORANGE      0xff8000    //
#define RGB_BLUE        0x0000FF    //   0   0 255
#define RGB_YELLOW      0xFFFF00    //
#define RGB_RED         0xFF0000    //   0 255 110
#define RGB_DARK_RED    0x110000    //   0 255 110
#define RGB_GREEN       0x00FF00    //  85 255 110
#define RGB_CYAN        0x00FFFF    //   0 255 255
#define RGB_TEAL        0x00FF80    //
#define RGB_PURPLE      0x9933FF    //
#define RGB_BLANK       0x101010    //  16  16  16
#define RGB_OFF         0x000000    //   0   0   0

// Some Pre-defined hue values
// TODO: remove HSV_ underscore once FastLED is gone
#define HSV_HUE_RED     0
#define HSV_HUE_ORANGE  32
#define HSV_HUE_YELLOW  64
#define HSV_HUE_GREEN   96
#define HSV_HUE_AQUA    128
#define HSV_HUE_BLUE    160
#define HSV_HUE_PURPLE  192
#define HSV_HUE_PINK    224

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

  bool empty() const;
  void clear();

  // public members
  union
  {
    struct
    {
      uint8_t hue;
      uint8_t sat;
      uint8_t val;
    };
    uint8_t raw[3];
  };
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

  bool empty() const;
  void clear();

  void adjustBrightness(uint8_t fadeBy);
  void serialize(ByteStream &buffer) const;
  void unserialize(ByteStream &buffer);

  // public members
  union
  {
    struct
    {
      uint8_t red;
      uint8_t green;
      uint8_t blue;
    };
    uint8_t raw[3];
  };
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
