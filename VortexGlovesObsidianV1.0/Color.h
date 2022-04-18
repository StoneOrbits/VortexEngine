#ifndef COLOR_H
#define COLOR_H

#include <inttypes.h>

// Pre defined hex HSV values
#define HSV_WHITE   0x00006E    // 0 0 110
#define HSV_ORANGE  0x14FF6E    // 20 255 110
#define HSV_BLUE    0xA0FF6E    // 160 255 110
#define HSV_YELLOW  0x3CFF6E    // 60  255 110
#define HSV_RED     0x00FF6E    // 0   255 110
#define HSV_GREEN   0x55FF6E    // 85  255 110
#define HSV_TEAL    0x78FF6E    // 120 255 110
#define HSV_PURPLE  0xD4FF6E    // 212 255 110
#define HSV_BLANK   0x000040    //   0   0  40
#define HSV_OFF     0x000000    //   0   0   0

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

// todo: remake color classes here
class HSVColor
{
  public:
    HSVColor() : raw_dword(0) {}
    HSVColor(uint32_t dwVal) : raw_dword(dwVal) {}
    HSVColor(uint8_t hue, uint8_t sat, uint8_t val) : 
      pad(0), hue(hue), sat(sat), val(val) {}

    // copy construction
    HSVColor(const HSVColor& rhs) {
      raw_dword = rhs.raw_dword;
    }
    // assignment operator
    HSVColor& operator= (const HSVColor& rhs) {
        raw_dword = rhs.raw_dword;
        return *this;
    }

    // public members
    union {
      struct {
          uint8_t pad;
          uint8_t hue;
          uint8_t sat;
          uint8_t val;
      };
      struct {
        uint8_t pad2;
        uint8_t raw_bytes[3];
      };
      uint32_t raw_dword;
    };
};

class RGBColor
{
  public:
    RGBColor() : raw_dword(0) {}
    RGBColor(uint32_t dwVal) : raw_dword(dwVal) {}
    RGBColor(uint8_t red, uint8_t green, uint8_t blue) : 
      pad(0), red(red), green(green), blue(blue) {}

    // copy construction
    RGBColor(const RGBColor& rhs) {
      raw_dword = rhs.raw_dword;
    }
    // assignment operator
    RGBColor& operator= (const RGBColor& rhs) {
      raw_dword = rhs.raw_dword;
      return *this;
    }
    // construction from HSV color
    RGBColor(const HSVColor& rhs) {
      // TODO: this
      //hsv2rgb_rainbow(CHSV(rhs.raw_dword), *this);
    }

    union {
      struct {
        uint8_t pad;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
      };
      struct {
        uint8_t pad2;
        uint8_t raw_bytes[3];
      };
      uint32_t raw_dword;
    };
};

#endif
