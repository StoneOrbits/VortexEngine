#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>

#include "../Colors/ColorTypes.h"
#include "LedTypes.h"

class LedStash;

class Leds
{
  // private unimplemented constructor
  Leds();

public:
  // opting for static class here because there should only ever be one
  // Led control object and I don't like singletons
  static bool init();
  static void cleanup();

  // control individual LED, these are appropriate to use in internal pattern logic
  static void set(RGBColor col);
  static void clear();

  // a very specialized api to hold all leds on a color for 250ms
  static void holdAll(RGBColor col);

  // get the RGBColor of an Led index
  static RGBColor getLed(LedPos pos) { return led(pos); }

  // actually update the LEDs and show the changes
  static void update();

private:
  // accessor for led colors, use this for all access to allow for mapping
  static inline RGBColor &led(LedPos pos)
  {
    if (pos > LED_LAST) {
      pos = LED_LAST;
    }
    // FLIP THE INDEXES because we want our enums to go from
    // PINKIE to INDEX for sake of simple iteration in menus
    // but the current hardware configuration is flipped
    return m_ledColors[LED_LAST - pos];
  }

  // the global brightness
  static uint8_t m_brightness;

  // array of led color values
  static RGBColor m_ledColors[LED_COUNT];
};

#endif
