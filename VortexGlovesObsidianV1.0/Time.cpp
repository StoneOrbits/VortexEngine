#include "Time.h"

#include <Arduino.h>

unsigned long g_curTime = 0;

void tickClock()
{
  g_curTime = millis();
}
