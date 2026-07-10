#include "TimeControl.h"

#include <math.h>

#include "../Memory/Memory.h"
#include "../Log/Log.h"

#include "Timings.h"

#include "../Leds/Leds.h"

#ifdef VORTEX_EMBEDDED
#include <avr/sleep.h>
#include <avr/interrupt.h>
#endif

#if !defined(_WIN32) || defined(WASM)
#include <unistd.h>
#include <time.h>
static uint64_t start_us = 0;
#define SEC_TO_US(sec) ((sec)*1000000)
#define NS_TO_US(ns) ((ns)/1000)
#else
#include <Windows.h>
static LARGE_INTEGER tps;
static LARGE_INTEGER start_us;
#endif

#if VARIABLE_TICKRATE == 1
uint32_t Time_tickrate = DEFAULT_TICKRATE;
#endif
uint32_t Time_curTick = 0;
#ifdef VORTEX_LIB
uint32_t Time_prevTime = 0;
uint32_t Time_firstTime = 0;
uint32_t Time_simulationTick = 0;
bool Time_isSimulation_ = false;
bool Time_instantTimestep = false;
#endif

#if VARIABLE_TICKRATE == 1
#define TICKRATE Time_tickrate
#else
#define TICKRATE DEFAULT_TICKRATE
#endif

bool Time_init(void)
{
#ifdef VORTEX_EMBEDDED
#if (F_CPU == 20000000)
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
#elif (F_CPU == 10000000)
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc));
#else
  #error "F_CPU not supported"
#endif
  _PROTECTED_WRITE(CPUINT_CTRLA, CPUINT_IVSEL_bm);
#endif
  Time_curTick = 0;
#if VARIABLE_TICKRATE == 1
  Time_tickrate = DEFAULT_TICKRATE;
#endif
#ifdef VORTEX_LIB
  Time_firstTime = Time_prevTime = Time_microseconds();
  Time_simulationTick = 0;
  Time_isSimulation_ = false;
  Time_instantTimestep = false;
#endif
#if !defined(_WIN32) || defined(WASM)
  start_us = Time_microseconds();
#else
  QueryPerformanceFrequency(&tps);
  QueryPerformanceCounter(&start_us);
#endif
  return true;
}

void Time_cleanup(void)
{
}

void Time_tickClock(void)
{
  Time_curTick++;

#if DEBUG_ALLOCATIONS == 1
  if ((Time_curTick % MS_TO_TICKS(1000)) == 0) {
    DEBUG_LOGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
  }
#endif

#ifdef VORTEX_LIB
  if (Time_instantTimestep) {
    return;
  }

  uint32_t elapsed_us;
  uint32_t us;
  do {
    us = Time_microseconds();
    if (us < Time_prevTime) {
      elapsed_us = (uint32_t)((UINT32_MAX - Time_prevTime) + us);
    } else {
      elapsed_us = (uint32_t)(us - Time_prevTime);
    }
#if !defined(_WIN32) && defined(VORTEX_LIB)
    uint32_t required = (1000000 / TICKRATE);
    uint32_t sleepTime = 0;
    if (required > elapsed_us) {
      sleepTime = required - elapsed_us;
    }
    Time_delayMicroseconds(sleepTime);
    break;
#endif
  } while (elapsed_us < (1000000 / TICKRATE));

  Time_prevTime = Time_microseconds();
#endif
}

uint32_t Time_getCurtime(void)
{
  return Time_curTick + SIMULATION_TICK;
}

uint32_t Time_getRealCurtime(void)
{
  return Time_curTick;
}

uint32_t Time_getTickrate(void)
{
  return TICKRATE;
}

void Time_setTickrate(uint32_t tickrate)
{
#if VARIABLE_TICKRATE == 1
  if (!tickrate) {
    tickrate = DEFAULT_TICKRATE;
  } else if (tickrate > 1000000) {
    tickrate = 1000000;
  }
  Time_tickrate = tickrate;
#endif
}

#if VARIABLE_TICKRATE == 1
uint32_t Time_millisecondsToTicks(uint32_t ms)
{
  if (!ms) {
    return 0;
  }
  uint32_t ticks = (ms * TICKRATE) / 1000;
  if (!ticks) {
    return 1;
  }
  return ticks;
}

uint32_t Time_secondsToTicks(uint32_t sec)
{
  return Time_millisecondsToTicks(sec * 1000);
}
#endif

uint32_t Time_microseconds(void)
{
#ifndef VORTEX_LIB
  uint32_t ticks;
  ticks = (Time_curTick * DEFAULT_TICKRATE) + (TCB0.CNT / 1000);
  return ticks;
#elif defined(_WIN32)
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  if (!tps.QuadPart) {
    return 0;
  }
  return (unsigned long)((now.QuadPart - start_us.QuadPart) * 1000000 / tps.QuadPart);
#else
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  uint64_t us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
  return (unsigned long)us;
#endif
}

#ifdef VORTEX_EMBEDDED
__attribute__ ((noinline))
#endif
void Time_delayMicroseconds(uint32_t us)
{
#ifdef VORTEX_EMBEDDED
#if F_CPU >= 20000000L
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"
    "nop" );
  if (us <= 1) return;
  us = us << 1;
  us -= 2;
#elif F_CPU >= 10000000L
  if (us <= 2) return;
  us -= 4;
#endif
  __asm__ __volatile__(
    "1: sbiw %0, 1" "\n\t"
    "rjmp .+0"      "\n\t"
    "rjmp .+0"      "\n\t"
    "rjmp .+0"      "\n\t"
    "brne 1b" : "=w" (us) : "0" (us)
  );
#elif defined(_WIN32)
  uint32_t newtime = Time_microseconds() + us;
  while (Time_microseconds() < newtime) {
  }
#else
  usleep(us);
#endif
}

void Time_delayMilliseconds(uint32_t ms)
{
#ifdef VORTEX_EMBEDDED
  for (uint16_t i = 0; i < ms; ++i) {
    Time_delayMicroseconds(1000);
  }
#elif defined(_WIN32)
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}

#ifdef VORTEX_LIB

uint32_t Time_startSimulation(void)
{
  Time_simulationTick = 0;
  Time_isSimulation_ = true;
  return (uint32_t)Time_getCurtime();
}

uint32_t Time_tickSimulation(void)
{
  return ++Time_simulationTick;
}

bool Time_isSimulation(void)
{
  return Time_isSimulation_;
}

uint32_t Time_getSimulationTick(void)
{
  return Time_simulationTick;
}

uint32_t Time_endSimulation(void)
{
  uint32_t endTick = (uint32_t)Time_getCurtime();
  Time_simulationTick = 0;
  Time_isSimulation_ = false;
  return endTick;
}

void Time_setInstantTimestep(bool instant)
{
  Time_instantTimestep = instant;
}

bool Time_isInstantStepping(void)
{
  return Time_instantTimestep;
}

#endif

#if TIMER_TEST == 1
#include <assert.h>

void Time_test(void)
{
  DEBUG_LOG("Starting Time class tests...");

  Time_setInstantTimestep(true);

  assert(Time_init());
  DEBUG_LOG("Init test passed");
  Time_cleanup();

  uint32_t initialTick = Time_curTick;
  Time_tickClock();
  assert(Time_curTick == initialTick + 1);
  DEBUG_LOG("tickClock test passed");

  assert(Time_millisecondsToTicks(1000) == TICKRATE);
  assert(Time_millisecondsToTicks(500) == TICKRATE / 2);
  assert(Time_millisecondsToTicks(0) == 0);
  DEBUG_LOG("msToTicks test passed");

  assert(Time_getRealCurtime() == Time_curTick);
  DEBUG_LOG("getRealCurtime test passed");

  assert(Time_getTickrate() == TICKRATE);
  DEBUG_LOG("getTickrate test passed");

#if VARIABLE_TICKRATE == 1
  uint32_t newTickrate = TICKRATE * 2;
  Time_setTickrate(newTickrate);
  assert(Time_getTickrate() == newTickrate);
  DEBUG_LOG("setTickrate test passed");
#endif

#ifdef VORTEX_LIB
  uint32_t simulationStartTick = Time_startSimulation();
  assert(Time_isSimulation());
  assert(Time_getSimulationTick() == 0);
  DEBUG_LOG("startSimulation test passed");

  uint32_t simulationTick = Time_tickSimulation();
  assert(Time_getSimulationTick() == 1);
  DEBUG_LOGF("tickSimulation test passed, simulationTick: %u", simulationTick);

  uint32_t simulationEndTick = Time_endSimulation();
  assert(!Time_isSimulation());
  assert(simulationEndTick == simulationStartTick + 1);
  DEBUG_LOGF("endSimulation test passed, simulationEndTick: %u", simulationEndTick);
#endif

  DEBUG_LOG("Time class tests completed successfully.");
}
#endif
