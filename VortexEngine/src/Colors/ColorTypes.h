#ifndef COLOR_H
#define COLOR_H

#include <inttypes.h>

#include "../VortexConfig.h"

// if this bit is present it's an HSV constant
#define HSV_BIT ((uint32_t)1 << 31)

// produce a DWORD HSV constant
#define HSV(h, s, v) (HSV_BIT | ((uint32_t)h << 16) | ((uint32_t)s << 8) | (uint32_t)v)

// Pre defined hex HSV values
#define HSV_WHITE   HSV_BIT | (uint32_t)0x00006E    // 0 0 110
#define HSV_ORANGE  HSV_BIT | (uint32_t)0x14FF6E    // 20 255 110
#define HSV_BLUE    HSV_BIT | (uint32_t)0xA0FF6E    // 160 255 110
#define HSV_YELLOW  HSV_BIT | (uint32_t)0x3CFF6E    // 60  255 110
#define HSV_RED     HSV_BIT | (uint32_t)0x00FF6E    // 0   255 110
#define HSV_GREEN   HSV_BIT | (uint32_t)0x55FF6E    // 85  255 110
#define HSV_TEAL    HSV_BIT | (uint32_t)0x78FF6E    // 120 255 110
#define HSV_PURPLE  HSV_BIT | (uint32_t)0xD4FF6E    // 212 255 110
#define HSV_BLANK   HSV_BIT | (uint32_t)0x000040    //   0   0  40
#define HSV_OFF     HSV_BIT | (uint32_t)0x000000    //   0   0   0

// Pure colors
#define RGB_WHITE       (uint32_t)0xFFFFFF    // 255 255 255
#define RGB_BLUE        (uint32_t)0x0000FF    //   0   0 255
#define RGB_YELLOW      (uint32_t)0xFFFF00    // 255 255   0
#define RGB_RED         (uint32_t)0xFF0000    // 255   0   0
#define RGB_GREEN       (uint32_t)0x00FF00    //   0 255   0
#define RGB_CYAN        (uint32_t)0x00FFFF    //   0 255 255
#define RGB_PURPLE      (uint32_t)0x9933FF    // 153  51 255
#define RGB_ORANGE      (uint32_t)0xFF8300    // 255 131   0
#define RGB_TEAL        (uint32_t)0x008080    //   0 128 128
#define RGB_OFF         (uint32_t)0x000000    //   0   0   0

// very low versions of each color
#define RGB_MIN_WHITE   (uint32_t)0x101010    //  16  16  16
#define RGB_MIN_BLUE    (uint32_t)0x000010    //   0   0  16
#define RGB_MIN_YELLOW  (uint32_t)0x101000    //  16  16   0
#define RGB_MIN_RED     (uint32_t)0x100000    //  16   0   0
#define RGB_MIN_GREEN   (uint32_t)0x001000    //   0  16   0
#define RGB_MIN_CYAN    (uint32_t)0x001010    //   0  16  16
#define RGB_MIN_PURPLE  (uint32_t)0x090310    //   9   3  16
#define RGB_MIN_ORANGE  (uint32_t)0x100800    //  16   8   0
#define RGB_MIN_TEAL    (uint32_t)0x001010    //   0   8   8

// Low versions of each color
#define RGB_LOW1_WHITE  (uint32_t)0x333333    //  51  51  51
#define RGB_LOW1_BLUE   (uint32_t)0x000033    //   0   0  51
#define RGB_LOW1_YELLOW (uint32_t)0x333300    //  51  51   0
#define RGB_LOW1_RED    (uint32_t)0x330000    //  51   0   0
#define RGB_LOW1_GREEN  (uint32_t)0x003300    //   0  51   0
#define RGB_LOW1_CYAN   (uint32_t)0x003333    //   0  51  51
#define RGB_LOW1_PURPLE (uint32_t)0x330033    //  51  51  51
#define RGB_LOW1_ORANGE (uint32_t)0x331900    //  51  25   0
#define RGB_LOW1_TEAL   (uint32_t)0x001919    //   0  25  25

#define RGB_LOW2_WHITE  (uint32_t)0x666666    // 102 102 102
#define RGB_LOW2_BLUE   (uint32_t)0x000066    //   0   0 102
#define RGB_LOW2_YELLOW (uint32_t)0x666600    // 102 102   0
#define RGB_LOW2_RED    (uint32_t)0x660000    // 102   0   0
#define RGB_LOW2_GREEN  (uint32_t)0x006600    //   0 102   0
#define RGB_LOW2_CYAN   (uint32_t)0x006666    //   0 102 102
#define RGB_LOW2_PURPLE (uint32_t)0x660066    // 102  51 102
#define RGB_LOW2_ORANGE (uint32_t)0x663300    // 102  51   0
#define RGB_LOW2_TEAL   (uint32_t)0x003333    //   0  51  51

#define RGB_LOW3_WHITE  (uint32_t)0x999999    // 153 153 153
#define RGB_LOW3_BLUE   (uint32_t)0x000099    //   0   0 153
#define RGB_LOW3_YELLOW (uint32_t)0x999900    // 153 153   0
#define RGB_LOW3_RED    (uint32_t)0x990000    // 153   0   0
#define RGB_LOW3_GREEN  (uint32_t)0x009900    //   0 153   0
#define RGB_LOW3_CYAN   (uint32_t)0x009999    //   0 153 153
#define RGB_LOW3_PURPLE (uint32_t)0x990099    // 153  51 153
#define RGB_LOW3_ORANGE (uint32_t)0x994C00    // 153  76   0
#define RGB_LOW3_TEAL   (uint32_t)0x004C4C    //   0  76  76

// Medium versions of each color
#define RGB_MED1_WHITE  (uint32_t)0xB3B3B3    // 179 179 179
#define RGB_MED1_BLUE   (uint32_t)0x0000B3    //   0   0 179
#define RGB_MED1_YELLOW (uint32_t)0xB3B300    // 179 179   0
#define RGB_MED1_RED    (uint32_t)0xB30000    // 179   0   0
#define RGB_MED1_GREEN  (uint32_t)0x00B300    //   0 179   0
#define RGB_MED1_CYAN   (uint32_t)0x00B3B3    //   0 179 179
#define RGB_MED1_PURPLE (uint32_t)0xB300B3    // 179  51 179
#define RGB_MED1_ORANGE (uint32_t)0xB36500    // 179 101   0
#define RGB_MED1_TEAL   (uint32_t)0x006565    //   0 101 101

#define RGB_MED2_WHITE  (uint32_t)0xCCCCCC    // 204 204 204
#define RGB_MED2_BLUE   (uint32_t)0x0000CC    //   0   0 204
#define RGB_MED2_YELLOW (uint32_t)0xCCCC00    // 204 204   0
#define RGB_MED2_RED    (uint32_t)0xCC0000    // 204   0   0
#define RGB_MED2_GREEN  (uint32_t)0x00CC00    //   0 204   0
#define RGB_MED2_CYAN   (uint32_t)0x00CCCC    //   0 204 204
#define RGB_MED2_PURPLE (uint32_t)0xCC00CC    // 204  51 204
#define RGB_MED2_ORANGE (uint32_t)0xCC7E00    // 204 126   0
#define RGB_MED2_TEAL   (uint32_t)0x007E7E    //   0 126 126

#define RGB_MED3_WHITE  (uint32_t)0xE6E6E6    // 230 230 230
#define RGB_MED3_BLUE   (uint32_t)0x0000E6    //   0   0 230
#define RGB_MED3_YELLOW (uint32_t)0xE6E600    // 230 230   0
#define RGB_MED3_RED    (uint32_t)0xE60000    // 230   0   0
#define RGB_MED3_GREEN  (uint32_t)0x00E600    //   0 230   0
#define RGB_MED3_CYAN   (uint32_t)0x00E6E6    //   0 230 230
#define RGB_MED3_PURPLE (uint32_t)0xE600E6    // 230 102 230
#define RGB_MED3_ORANGE (uint32_t)0xE69700    // 230 151   0
#define RGB_MED3_TEAL   (uint32_t)0x009797    //   0 151 151

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

  void adjustBrightness(uint8_t fadeBy);
  void serialize(ByteStream &buffer) const;
  void unserialize(ByteStream &buffer);

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
