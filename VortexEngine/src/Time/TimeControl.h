#ifndef TIME_CONTROL_H
#define TIME_CONTROL_H

#include <stdbool.h>
#include <inttypes.h>
#include <time.h>

#include "../VortexConfig.h"
#include "../Leds/LedTypes.h"

#if VARIABLE_TICKRATE == 1
#define MS_TO_TICKS(ms) Time_millisecondsToTicks(ms)
#define SEC_TO_TICKS(s) Time_secondsToTicks(s)
#else
#define MS_TO_TICKS(ms) (uint32_t)(((uint32_t)(ms) * DEFAULT_TICKRATE) / 1000)
#define SEC_TO_TICKS(s) (uint32_t)((uint32_t)(s) * DEFAULT_TICKRATE)
#endif

#ifdef VORTEX_LIB
#define SIMULATION_TICK Time_getSimulationTick()
#else
#define SIMULATION_TICK 0
#endif

// Time is a stateless namespace of free functions, there should only ever
// be one instance of it and the tick state lives in TimeControl.cpp

// opting for static class here because there should only ever be one
// Settings control object and I don't like singletons
bool Time_init(void);
void Time_cleanup(void);

// tick the clock forward to millis()
void Time_tickClock(void);

// get the current tick, offset by any active simulation (simulation only exists in vortexlib)
// Exposing this in the header seems to save on space a non negligible amount, it is used a lot
// and exposing in the header probably allows the compiler to optimize away repititive calls
static inline uint32_t Time_getCurtime(void);

// this ignore simulation time, it's used by timers to make simulations work
uint32_t Time_getRealCurtime(void);

// Set tickrate in Ticks Per Second (TPS)
// The valid range for this is 1 <= x <= 1000000
//
// Setting a value of 0 will restore the default
//
// NOTE: Patterns will not change when this changes, you
//       must re-create the pattern for the change to take
//       effect. This is done by design to allow the test
//       framework to control the speed of patterns.
//       See PatternBuilder.cpp for more info.
void Time_setTickrate(uint32_t tickrate);

// The current tickrate
uint32_t Time_getTickrate(void);

// these apis are only present if the tickrate can change
#if VARIABLE_TICKRATE == 1
// convert milliseconds to a tickcount based on tickrate
uint32_t Time_millisecondsToTicks(uint32_t ms);

// convert seconds to a tickcount based on tickrate
static inline uint32_t Time_secondsToTicks(uint32_t sec)
{
  return Time_millisecondsToTicks(sec * 1000);
}
#endif

// Current microseconds since startup, only use this for things like measuring rapid data transfer timings.
// If you just need to perform regular time checks for a pattern or some logic then use Time_getCurtime() and measure
// time in ticks, use the SEC_TO_TICKS() or MS_TO_TICKS() macros to convert timings to measures of ticks for
// purpose of comparing against Time_getCurtime()
uint32_t Time_microseconds(void);

// delay for some number of microseconds or milliseconds, these are bad
void Time_delayMicroseconds(uint32_t us);
void Time_delayMilliseconds(uint32_t ms);

#ifdef VORTEX_LIB
// Start a time simulation, while the simulation is active you can
// increment the 'current time' with Time_tickSimulation() and all calls
// to Time_getCurtime will reflect the changes, then when you call
// endSimulation() the currentTime will be restored
uint32_t Time_startSimulation(void);

// Tick a time simulation forward, returning the next tick
uint32_t Time_tickSimulation(void);

// Whether running a time simulation
bool Time_isSimulation(void);

// get the current tick in the simulation
uint32_t Time_getSimulationTick(void);

// Finish a time simulation
uint32_t Time_endSimulation(void);

// Toggle instant step
void Time_setInstantTimestep(bool instant);
bool Time_isInstantStepping(void);
#endif

#if TIMER_TEST == 1
void Time_test(void);
#endif

#ifdef VORTEX_LIB
static inline uint32_t Time_getCurtime(void)
{
  return Time_getRealCurtime() + SIMULATION_TICK;
}
#else
static inline uint32_t Time_getCurtime(void)
{
  return Time_getRealCurtime();
}
#endif

#endif
