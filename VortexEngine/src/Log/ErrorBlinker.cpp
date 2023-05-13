#include "ErrorBlinker.h"

#if VORTEX_ERROR_BLINK == 1

#include "../Leds/Leds.h"

#include <Arduino.h>

static uint16_t g_error = ERROR_NONE;

void setError(uint16_t err)
{
  if (g_error != ERROR_NONE) {
    // do not overwrite errors, the first error gets precedent
    return;
  }
  g_error = err;
}

uint16_t getError()
{
  return g_error;
}

void blinkError()
{
  // this is so bad ugh it blinks r -> g -> b for the error code like
  // error 231 is 2 reds, 3 greens, 1 blue
  if (g_error == ERROR_NONE) {
    return;
  }
  uint8_t errOnes = (g_error % 10);
  uint8_t errTens = ((g_error - errOnes) % 100) / 10;
  uint8_t errHunds = (g_error - (errTens + errOnes)) / 100;
  Leds::setIndex(LED_1, RGBColor(40, 40, 40));
  const uint32_t msWait = 500;
  for (uint8_t i = 0; i < errHunds; ++i) {
    Leds::setIndex(LED_0, RGBColor(255, 0, 0)); Leds::update(); delay(msWait);
    Leds::clearIndex(LED_0); Leds::update(); delay(msWait);
  }
  for (uint8_t i = 0; i < errTens; ++i) {
    Leds::setIndex(LED_0, RGBColor(0, 255, 0)); Leds::update(); delay(msWait);
    Leds::clearIndex(LED_0); Leds::update(); delay(msWait);
  }
  for (uint8_t i = 0; i < errOnes; ++i) {
    Leds::setIndex(LED_0, RGBColor(0, 0, 255)); Leds::update(); delay(msWait);
    Leds::clearIndex(LED_0); Leds::update(); delay(msWait);
  }
  delay(msWait);
}

#endif
