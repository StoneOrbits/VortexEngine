#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#include "ColorTypes.h"
#include "LedConfig.h"

// the starting default brightness
#define DEFAULT_BRIGHTNESS 255

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

  // control two LEDs on a finger, these are appropriate for use in internal pattern logic
  static void setFinger(Finger finger, RGBColor col);
  static void setFingers(Finger first, Finger last, RGBColor col);

  // Turn off both LEDs on a finger, these are appropriate for use in internal pattern logic
  static void clearFinger(Finger finger) { setFinger(finger, HSV_OFF); }
  static void clearFingers(Finger first, Finger last) { setFingers(first, last, HSV_OFF); }

  // Blink an led to blank or a color
  // Switch the color to make it blink to another color
  // These functions are intended to be used in menus, they are *not*
  // appropriate for internal pattern usage.  These functions do not 
  // utilize the tick offset for each finger so all fingers will blink 
  // in sync when these functions are used
  static void blinkIndex(LedPos target, uint32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);
  static void blinkRange(LedPos first, LedPos last, uint32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);
  static void blinkAll(int32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);
  // Blink both LEDs on a finger
  static void blinkFinger(Finger finger, uint32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);
  static void blinkFingers(Finger first, Finger last, uint32_t offMs = 250, uint32_t onMs = 500, RGBColor col = RGB_OFF);

  // global brightness
  static uint32_t getBrightness() { return m_brightness; }
  static void setBrightness(uint32_t brightness) { m_brightness = brightness; }

  // actually update the LEDs and show the changes
  static void update();

private:
  static void clearOnboardLED();

  // the global brightness
  static uint32_t m_brightness;

  // array of led color values
  static RGBColor m_ledColors[LED_COUNT];

  // the onboard LED on the adafruit board
  static Adafruit_DotStar m_onboardLED;
};

#endif
