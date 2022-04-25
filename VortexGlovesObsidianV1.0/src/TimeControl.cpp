#include "TimeControl.h"

#include <Arduino.h>

// should only be one time control
TimeControl *g_pTimeControl = nullptr;

TimeControl::TimeControl() :
  m_curTime(0)
{
}

TimeControl::~TimeControl()
{
  g_pTimeControl = nullptr;
}

bool TimeControl::init()
{
  if (g_pTimeControl) {
    // programmer error, only one time control
    return false;
  }
  g_pTimeControl = this;
  return true;
}

void TimeControl::tickClock()
{
  //m_curTime = millis();
  m_curTime++;
}

// get the current time with optional led position time offset
uint64_t TimeControl::getCurtime(LedPos pos) const
{
  // basic time offset of 33ms per led
  return m_curTime + (3*pos);// + (33 * pos);// + (33 * (uint32_t)pos);
}
