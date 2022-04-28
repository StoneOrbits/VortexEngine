#include "TimeControl.h"

#include <Arduino.h>
#include <math.h>

#include "Timings.h"
#include "Log.h"

// static members
uint64_t Time::m_curTick = 0;
uint64_t Time::m_prevTime = 0;
uint32_t Time::m_tickrate = DEFAULT_TICKRATE;
uint32_t Time::m_tickOffset = DEFAULT_TICK_OFFSET;

#ifdef FIXED_TICKRATE
#define TICKRATE DEFAULT_TICKRATE
#else
#define TICKRATE m_tickrate
#endif

bool Time::init()
{
  m_prevTime = micros();
  return true;
}

void Time::tickClock()
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
  } while (elapsed_us < (1000000 / TICKRATE));

  // store current time
  m_prevTime = micros();
}

// get the current time with optional led position time offset
uint64_t Time::getCurtime(LedPos pos)
{
  // basic time offset of 33ms per led
  return m_curTick + (pos * m_tickOffset);
}

// Set tickrate in Ticks Per Second (TPS)
// The valid range for this is 1 <= x <= 1000000
void Time::setTickrate(uint32_t tickrate)
{
#ifndef FIXED_TICKRATE
  if (!tickrate) {
    // can't set 0 tickrate, so 0 sets default
    tickrate = DEFAULT_TICKRATE;
  } else if (tickrate > 1000000) {
    // more than 1 million ticks per second won't work anyway
    tickrate = 1000000;
  }
  // update the tickrate
  m_tickrate = tickrate;
#endif
}

void Time::setTickOffset(uint32_t tickOffset)
{
  m_tickOffset = tickOffset;
}

uint32_t Time::msToTicks(uint32_t ms)
{
  // 0ms = 0 ticks
  if (!ms) {
    return 0;
  }
  // but anything > 0 ms must be no less than 1 tick
  // otherwise short durations will disappear at low
  // tickrates
  uint32_t ticks = (ms * TICKRATE) / 1000;
  if (!ticks) {
    return 1;
  }
  return ticks;
}
