#include "FastLED.h"

// global instance
FastLEDClass FastLED;

// called when the user calls FastLED.addLeds
void FastLEDClass::init(CRGB *cl, int count)
{
#ifndef LINUX_FRAMEWORK
#endif
}

// called when user calls FastLED.setBrightness
void FastLEDClass::setBrightness(int brightness)
{
#ifndef LINUX_FRAMEWORK
#endif
}

// called when user calls FastLED.show
void FastLEDClass::show(uint32_t brightness)
{
  setBrightness(brightness);
}

