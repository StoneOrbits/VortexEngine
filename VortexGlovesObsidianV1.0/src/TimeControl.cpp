#include "TimeControl.h"

#include <Arduino.h>

#include "Log.h"

// should only be one time control
TimeControl *g_pTimeControl = nullptr;

// the ticks per millisecond
#define MS_PER_TICK 1

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
  // tick clock forward
  m_curTime++;

  // perform timestep
  static uint64_t lastshow = micros();
  uint64_t elapsed_ms;
  do {
      elapsed_ms = (micros() - lastshow);
  } while (elapsed_ms < MS_PER_TICK * 1000);
  lastshow = micros();
}

// get the current time with optional led position time offset
uint64_t TimeControl::getCurtime(LedPos pos) const
{
  // basic time offset of 33ms per led
  return m_curTime ;// + (33 * (uint32_t)pos);
}
