#include "tinyNeoPixel_Static.h"

#include "VortexLib.h"

tinyNeoPixel::tinyNeoPixel()
{
}

tinyNeoPixel::tinyNeoPixel(int count, int b, int c, void *leds)
{
  Vortex::vcallbacks()->ledsInit(leds, count);
}

void tinyNeoPixel::show()
{
  Vortex::vcallbacks()->ledsShow();
}