#include "TimeControl.h"

#include <Arduino.h>

TimeControl::TimeControl() :
  m_curTime(0)
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

// get the current time with optional led position time offset
uint64_t TimeControl::getCurtime(LedPos pos) const
{
  // basic time offset of 33ms per led
  return m_curTime + (33 * (uint32_t)pos);
}
