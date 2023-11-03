#include <math.h>

#include "LedStash.h"
#include "Leds.h"

#include "../Time/TimeControl.h"

#include "../VortexConfig.h"

#ifdef VORTEX_LIB
#include "../../VortexLib/VortexLib.h"
#endif

#ifdef VORTEX_EMBEDDED
#define LED_DATA_PIN  4
#define POWER_LED_PIN 7
#endif

// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };
// global brightness
uint8_t Leds::m_brightness = DEFAULT_BRIGHTNESS;

bool Leds::init()
{
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsInit(m_ledColors, LED_COUNT);
#endif
  return true;
}

void Leds::cleanup()
{
  for (uint8_t i = 0; i < LED_COUNT; ++i) {
    m_ledColors[i].clear();
  }
}

void Leds::set(RGBColor col)
{
  led(LED_0) = col;
}

void Leds::clear()
{
  led(LED_0) = 0;
}

void Leds::holdAll(RGBColor col)
{
  set(col);
  update();
  Time::delayMilliseconds(250);
}

void Leds::update()
{
#ifdef VORTEX_EMBEDDED
#endif
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsShow();
#endif
}
