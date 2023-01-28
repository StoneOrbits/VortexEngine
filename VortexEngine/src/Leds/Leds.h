#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>

#include <FastLED.h>
#include <Adafruit_DotStar.h>

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
  static void setIndex(LedPos target, RGBColor col);
  static void setRange(LedPos first, LedPos last, RGBColor col);
  static void setAll(RGBColor col);

  // Turn off individual LEDs, these are appropriate to use in internal pattern logic
  static void clearIndex(LedPos target) { setIndex(target, HSV_OFF); }
  static void clearRange(LedPos first, LedPos last) { setRange(first, last, HSV_OFF); }
  static void clearAll() { setAll(HSV_OFF); }

  // control all evens/odds
  static void setAllEvens(RGBColor col);
  static void setAllOdds(RGBColor col);

  // clear evens/odds
  static void clearAllEvens();
  static void clearAllOdds();

  // Control full quadrants of Leds
  static void setQuadrant(Quadrant quadrant, RGBColor col);
  static void setQuadrants(Quadrant first, Quadrant last, RGBColor col);
  static void setQuadrantFive(RGBColor col);

  // Turn off full quadrants
  static void clearQuadrant(Quadrant quadrant) { setRange(quadrantFirstLed(quadrant), quadrantLastLed(quadrant), HSV_OFF); }
  static void clearQuadrants(Quadrant first, Quadrant last) { setRange(quadrantFirstLed(first), quadrantLastLed(last), HSV_OFF); }
  static void clearQuadrantFive();

  // set a ring of leds
  static void setRing(Ring ring, RGBColor col);
  static void setRings(Ring first, Ring last, RGBColor col);

  // clear a ring of leds
  static void clearRing(Ring ring);
  static void clearRings(Ring first, Ring last);

  // Control pairs of Leds, for menu display
  static void setPair(LedPair pair, RGBColor col);

  // Turn off pairs of Leds
  static void clearPair(LedPair pair);

  // Turn on/off a mapping of leds with a color
  static void setMap(LedMap map, RGBColor col);
  static void clearMap(LedMap map);

  // stores Led for later use
  static void stashAll(LedStash &stash);

  // restores Leds from stash
  static void restoreAll(const LedStash &stash);

  // Dim individual LEDs, these are appropriate to use in internal pattern logic
  static void adjustBrightnessIndex(LedPos target, uint8_t fadeBy);
  static void adjustBrightnessRange(LedPos first, LedPos last, uint8_t fadeBy);
  static void adjustBrightnessAll(uint8_t fadeBy);

  // Blink an led to blank or a color
  //
  // These APIs work by checking if the current time is within the
  // 'on duration' then they apply the color to the target, otherwise they do
  // nothing. The goal of this behaviour is to allow them to be used in any
  // situation to 'blink' an led to either off or some color.
  //
  // However since these APIs modulate current time to check if within the 'on'
  // threshold that makes them unsuitable for internal pattern usage because it
  // is unpredictable whether they will blink on or off first
  static void blinkIndex(LedPos target, uint64_t time, uint32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);
  static void blinkRange(LedPos first, LedPos last, uint64_t time, uint32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);
  static void blinkQuadrant(Quadrant target, uint64_t time, uint32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);
  static void blinkQuadrantFive(uint64_t time, uint32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);
  static void blinkAll(uint64_t time, int32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);

  // breath the hue on an index
  // warning: uses hsv to rgb in realtime!
  static void breathIndex(LedPos target, uint32_t hue, uint32_t variance,
    uint32_t magnitude = 15, uint8_t sat = 255, uint8_t val = 210);

  static void breathQuadrant(Quadrant target, uint32_t hue, uint32_t variance,
    uint32_t magnitude = 15, uint8_t sat = 255, uint8_t val = 210);

  static void breathQuadrantFive(uint32_t hue, uint32_t variance,
    uint32_t magnitude = 15, uint8_t sat = 255, uint8_t val = 210);

  // get the RGBColor of an Led index
  static RGBColor getLed(LedPos pos) { return led(pos); }

  // global brightness
  static uint32_t getBrightness() { return m_brightness; }
  static void setBrightness(uint32_t brightness) { m_brightness = brightness; }

  // actually update the LEDs and show the changes
  static void update();

private:
  static void clearOnboardLED();

  // accessor for led colors, use this for all access to allow for mapping
  static inline RGBColor &led(LedPos pos)
  {
    if (pos > LED_LAST) {
      pos = LED_LAST;
    }
    // FLIP THE INDEXES because we want our enums to go from
    // PINKIE to INDEX for sake of simple iteration in menus
    // but the current hardware configuration is flipped
    return m_ledColors[pos];
  }

  // the global brightness
  static uint32_t m_brightness;

  // array of led color values
  static RGBColor m_ledColors[LED_COUNT];

  // the onboard LED on the adafruit board
  static Adafruit_DotStar m_onboardLED;
};

#endif
