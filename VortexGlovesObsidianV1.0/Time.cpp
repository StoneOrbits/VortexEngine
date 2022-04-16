#include "Time.h"

#include <Arduino.h>

extern unsigned long g_curTime;

void tickClock()
{
  g_curTime = millis();
}
