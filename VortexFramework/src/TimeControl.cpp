#include "TimeControl.h"

#include <Arduino.h>
#include <math.h>

#include "Infrared.h"
#include "Timings.h"
#include "Memory.h"
#include "Log.h"

// static members
uint64_t Time::m_curTick = 0;
uint64_t Time::m_prevTime = 0;
uint64_t Time::m_firstTime = 0;
uint32_t Time::m_tickrate = DEFAULT_TICKRATE;
uint32_t Time::m_tickOffset = DEFAULT_TICK_OFFSET;
uint32_t Time::m_simulationTick = 0;
bool Time::m_isSimulation = false;

#ifdef FIXED_TICKRATE
#define TICKRATE DEFAULT_TICKRATE
#else
#define TICKRATE m_tickrate
#endif

bool Time::init()
{
  m_firstTime = m_prevTime = micros();
  return true;
}

void Time::cleanup()
{
}

#include "Infrared.h"

void Time::tickClock()
{
  // tick clock forward
  m_curTick++;

#ifdef DEBUG_ALLOCATIONS
  if ((m_curTick % msToTicks(1000)) == 0) {
    DEBUG_LOGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
  }
#endif

  // perform timestep
  uint32_t elapsed_us;
  uint32_t us;
  do {
    us = micros();
    // detect rollover of microsecond counter
    if (us < m_prevTime) {
      // calculate wrapped around difference
      elapsed_us = (uint32_t)((UINT32_MAX - m_prevTime) + us);
    } else {
      // otherwise calculate regular difference
      elapsed_us = (uint32_t)(us - m_prevTime);
    }
    // 1000us per ms, divided by tickrate gives
    // the number of microseconds per tick
  } while (elapsed_us < (1000000 / TICKRATE));

  // store current time
  m_prevTime = micros();
}

// get the current time with optional led position time offset
uint64_t Time::getCurtime(LedPos pos)
{
  // the current tick, plus the time offset per LED, plus any
  // simulation offset
  return m_curTick + getTickOffset(pos) + getSimulationTick();
}

// the real current time, bypass simulations, used by timers
uint64_t Time::getRealCurtime()
{
  return m_curTick;
}

// get the amount of ticks this led position runs out of sync
uint32_t Time::getTickOffset(LedPos pos)
{
  return (pos * m_tickOffset);
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

// Start a time simulation, while the simulation is active you can
// increment the 'current time' with tickSimulation() then when you
// call endSimulation() the currentTime will be restored
uint32_t Time::startSimulation()
{
  m_simulationTick = 0;
  m_isSimulation = true;
  return (uint32_t)getCurtime();
}

// Tick a time simulation forward, returning the next tick
uint32_t Time::tickSimulation()
{
  return ++m_simulationTick;
}

// whether running time simulation
bool Time::isSimulation()
{
  return m_isSimulation;
}

// get the current tick in the simulation
uint32_t Time::getSimulationTick()
{
  return m_simulationTick;
}

// Finish a time simulation
uint32_t Time::endSimulation()
{
  uint32_t endTick = (uint32_t)getCurtime();
  m_simulationTick = 0;
  m_isSimulation = false;
  return endTick;
}

