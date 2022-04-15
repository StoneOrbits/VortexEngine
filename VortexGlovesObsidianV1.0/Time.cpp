#include "Time.h"

#include <Arduino.h>

extern unsigned long g_curTime;

void tick()
{
  g_curTime = millis();
}
