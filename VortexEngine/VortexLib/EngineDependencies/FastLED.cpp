#include "FastLED.h"
#include "VortexLib.h"

// global instance
FastLEDClass FastLED;

// called when the user calls FastLED.addLeds
void FastLEDClass::init(CRGB *cl, int count)
{
  Vortex::vcallbacks()->ledsInit(cl, count);
}

// called when user calls FastLED.setBrightness
void FastLEDClass::setBrightness(int brightness)
{
  Vortex::vcallbacks()->ledsBrightness(brightness);
}

// called when user calls FastLED.show
void FastLEDClass::show(uint32_t brightness)
{
  Vortex::vcallbacks()->ledsBrightness(brightness);
  Vortex::vcallbacks()->ledsShow();
}

