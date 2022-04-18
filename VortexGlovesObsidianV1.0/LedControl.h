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

    // turn on the power LED on the device
    void turnOnPowerLED();

    // control individual LED
    void setIndex(int target, CHSV col);
    // control range of LEDS
    void setRange(int first, int last, CHSV col);
    // clear all LEDs to a color (default off)
    void clearAll(CHSV col = HSV_OFF);

    // actually update the LEDs and show the changes
    void update();

  private:
    // array of led color values
    std::vector<CRGB> m_leds;

    // global brightness
    uint32_t m_brightness;

    // the power LED
    Adafruit_DotStar m_powerLED;
};

#endif
