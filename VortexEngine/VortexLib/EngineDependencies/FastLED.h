#pragma once

// for CRGB?
#ifdef LINUX_FRAMEWORK
#include <inttypes.h>
typedef struct tagCRGB
{
    uint8_t    bRed;
    uint8_t    bGreen;
    uint8_t    bBlue;
    uint8_t    bExtra;
} CRGB;           /* RGB Color */
#else
#include <Windows.h>
#endif

#include <inttypes.h>

#include "Arduino.h"

#define NEOPIXEL 1
#define DOTSTAR 1
#define BGR 1
//LED_DATA_PIN>

// =======================================================
// begin stolen code from fastled

inline void hsv2rgb_rainbow(uint32_t a, void *b) {}

#define __attribute__(x)

/// Representation of an HSV pixel (hue, saturation, value (aka brightness)).
struct CHSV
{
  union
  {
    struct
    {
      union
      {
        uint8_t hue;
        uint8_t h;
      };
      union
      {
        uint8_t saturation;
        uint8_t sat;
        uint8_t s;
      };
      union
      {
        uint8_t value;
        uint8_t val;
        uint8_t v;
      };
    };
    uint8_t raw[3];
  };

  /// default values are UNITIALIZED
  inline CHSV() __attribute__((always_inline)) :
    h(0), s(0), v(0)
  {
  }

  /// allow construction from H, S, V
  inline CHSV(uint8_t ih, uint8_t is, uint8_t iv) __attribute__((always_inline))
    : h(ih), s(is), v(iv)
  {
  }

  /// allow copy construction
  inline CHSV(const CHSV &rhs) __attribute__((always_inline))
  {
    h = rhs.h;
    s = rhs.s;
    v = rhs.v;
  }
};

/// Pre-defined hue values for HSV objects
typedef enum
{
  HUE_RED = 0,
  HUE_ORANGE = 32,
  HUE_YELLOW = 64,
  HUE_GREEN = 96,
  HUE_AQUA = 128,
  HUE_BLUE = 160,
  HUE_PURPLE = 192,
  HUE_PINK = 224
} HSVHue;

// ============================================================
// end stolen code from fastled

class FastLEDClass
{
public:
  // empty template to match existing fastled addLeds api
  template <int a, int b>
  void addLeds(CRGB *cl, int count)
  {
    init(cl, count);
  }

  template <int a, int b, int c, int d>
  void addLeds(CRGB *cl, int count)
  {
    init(cl, count);
  }

  // set refresh rate (does nothing)
  void setMaxRefreshRate(uint32_t rate, bool constrain) {}

  // internal 'init' function that is called when addLeds is called
  void init(CRGB *cl, int count);

  // to handle setBrightness call
  void setBrightness(int brightness);

  // handler for 'show' the LEDs
  void show(uint32_t brightness = 255);

private:
};

extern FastLEDClass FastLED;
