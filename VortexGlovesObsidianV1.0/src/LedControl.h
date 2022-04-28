#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#include "ColorTypes.h"
#include "LedConfig.h"

class Leds
{
  // private unimplemented constructor
  Leds();

public:
  // opting for static class here because there should only ever be one
  // Led control object and I don't like singletons
  static bool init();

  // control individual LED
  static void setIndex(LedPos target, RGBColor col);
  static void setRange(LedPos first, LedPos last, RGBColor col);
  static void setAll(RGBColor col);

  // Turn off individual LEDs
  static void clearIndex(LedPos target) { setIndex(target, HSV_OFF); }
  static void clearRange(LedPos first, LedPos last) { setRange(first, last, HSV_OFF); }
  static void clearAll() { setAll(HSV_OFF); }

  // Blink an led to blank or a color
  // The 'offMs' is how many milliseconds out of 1000 the LED will be 'off'
  // Switch the color to make it blink to another color
  // These functions are intended to be used in menus to indicate selection,
  // they are not appropriate for internal pattern usage.
  // These functions all operate on the same curtime which means all LEDs
  // will blink in sync when these functions are used
  static void blinkIndex(LedPos target, uint32_t offMs = 250, RGBColor col = RGB_OFF);
  static void blinkRange(LedPos first, LedPos last, uint32_t offMs = 250, RGBColor col = RGB_OFF);
  static void blinkAll(int32_t offMs = 250, RGBColor col = RGB_OFF);

  // control two LEDs on a finger
  static void setFinger(Finger finger, RGBColor col);
  static void setFingers(Finger first, Finger last, RGBColor col);

  // Turn off both LEDs on a finger
  static void clearFinger(Finger finger) { setFinger(finger, HSV_OFF); }
  static void clearFingers(Finger first, Finger last) { setFingers(first, last, HSV_OFF); }

  // Blink both LEDs on a finger
  static void blinkFinger(Finger finger, uint32_t offMs = 250, RGBColor col = RGB_OFF);
  static void blinkFingers(Finger first, Finger last, uint32_t offMs = 250, RGBColor col = RGB_OFF);

  // actually update the LEDs and show the changes
  static void update();

private:
  static void clearOnboardLED();
  static void blinkIndexInternal(LedPos target, RGBColor col);
  static void blinkRangeInternal(LedPos first, LedPos last, RGBColor col);

  // array of led color values
  static RGBColor m_ledColors[LED_COUNT];

  // the onboard LED on the adafruit board
  static Adafruit_DotStar m_onboardLED;
};

#endif
