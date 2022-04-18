#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>
#include <vector>

#include <Adafruit_DotStar.h>

#include "Color.h"

class LedControl
{
  public:
    LedControl();

    bool init();

    // control individual LED
    void setIndex(int target, RGBColor col);
    // control range of LEDS
    void setRange(int first, int last, RGBColor col);
    // clear all LEDs to a color (default off)
    void clearAll(RGBColor col = HSV_OFF);

    // actually update the LEDs and show the changes
    void update();

  private:
    void clearOnboardLED();

    // array of led color values
    std::vector<RGBColor> m_leds;

    // global brightness
    uint32_t m_brightness;

    // the onboard LED on the adafruit board
    Adafruit_DotStar m_onboardLED;
};

#endif
