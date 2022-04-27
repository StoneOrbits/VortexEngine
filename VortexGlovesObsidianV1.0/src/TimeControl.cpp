#include "TimeControl.h"

#include <Arduino.h>
#include <math.h>

#include "Log.h"

// should only be one time control
TimeControl *g_pTimeControl = nullptr;

TimeControl::TimeControl() :
  m_curTick(0),
  m_prevTime(0),
  m_tickrate(DEFAULT_TICKRATE),
  m_timeOffset(DEFAULT_TIME_OFFSET)
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
  m_prevTime = micros();
  g_pTimeControl = this;
  return true;
}

void TimeControl::tickClock()
{
  // tick clock forward
  m_curTick++;

  // perform timestep
  // wait till the elapsed frametime sufficient
  uint64_t elapsed_us;
  do {
    elapsed_us = (micros() - m_prevTime);
    // 1000us per ms, divided by tickrate gives
    // the number of microseconds per tick
  } while (elapsed_us < (1000000 / m_tickrate));

  // store current time
  m_prevTime = micros();
}

// get the current time with optional led position time offset
uint64_t TimeControl::getCurtime(LedPos pos) const
{
  // basic time offset of 33ms per led
  return m_curTick + (pos * m_timeOffset);
}

// Set tickrate in Ticks Per Second (TPS)
// The valid range for this is 1 <= x <= 1000000
void TimeControl::setTickrate(uint32_t tickrate)
{
  // no larger than 1 million will work
  if (tickrate > 1000000) {
    m_tickrate = 1000000;
  } else {
    m_tickrate = tickrate;
  }
}

void TimeControl::setTimeOffset(uint32_t timeOffset)
{
  m_timeOffset = timeOffset;
}

uint32_t TimeControl::msToTicks(uint32_t ms) const
{
  // 0ms = 0 ticks
  if (!ms) {
    return 0;
  }
  // but anything > 0 ms must be no less than 1 tick
  // otherwise short durations will disappear at low
  // tickrates
  uint32_t ticks = (ms * m_tickrate) / 1000;
  if (!ticks) {
    return 1;
  }
  return ticks;
}
