#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>

#include <FastLED.h>
#include <Adafruit_DotStar.h>

#include "ColorTypes.h"
#include "LedConfig.h"

class LedControl
{
  public:
    LedControl();

    bool init();

    // control individual LED
    void setIndex(LedPos target, RGBColor col);
    // control range of LEDs
    void setRange(LedPos first, LedPos last, RGBColor col);
    // set all LEDs
    void setAll(RGBColor col);

    // turn off an individual LED
    void clearIndex(LedPos target) { setIndex(target, HSV_OFF); }
    // turn off a range of LEDs
    void clearRange(LedPos first, LedPos last) { setRange(first, last, HSV_OFF); }
    // turn off all LEDs
    void clearAll() { setAll(HSV_OFF); }

    // actually update the LEDs and show the changes
    void update();

  private:
    void clearOnboardLED();

    // array of led color values
    CRGB m_ledColors[LED_COUNT];

    // global brightness
    uint32_t m_brightness;

    // the onboard LED on the adafruit board
    Adafruit_DotStar m_onboardLED;
};

#endif
