#include "TimeControl.h"

#include <Arduino.h>

TimeControl::TimeControl() :
  m_curtime(0)
{
}

bool TimeControl::init()
{
  // nothing for now
  return true;
}

void TimeControl::tickClock()
{
  m_curTime = millis();
}

uint64_t TimeControl::getCurtime(uint32_t finger)
{
  // basic time offset of 33ms per finger
  return m_curTime + (33 * finger);
}
