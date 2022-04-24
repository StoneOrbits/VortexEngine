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
    ~LedControl();

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

    // control two LEDs on a finger
    void setFinger(Finger finger, RGBColor col);
    // control two LEDs on each of a range of fingers
    void setFingers(Finger first, Finger last, RGBColor col);

    // clear both LEDs on a finger
    void clearFinger(Finger finger) { setFinger(finger, HSV_OFF); }
    // clear both LEDs on a range of fingers
    void clearFingers(Finger first, Finger last) { setFingers(first, last, HSV_OFF); }
    
    // actually update the LEDs and show the changes
    void update();

  private:
    void clearOnboardLED();

    // array of led color values
    CRGB m_ledColors[LED_COUNT];

    // the onboard LED on the adafruit board
    Adafruit_DotStar m_onboardLED;
};

// easy access to the led control
extern LedControl *g_pLedControl;

#endif
