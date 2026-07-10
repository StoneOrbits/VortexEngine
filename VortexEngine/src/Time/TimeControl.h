#ifndef TIME_CONTROL_H
#define TIME_CONTROL_H

#include <inttypes.h>
#include <stdbool.h>
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

bool Time_init(void);
void Time_cleanup(void);
void Time_tickClock(void);
uint32_t Time_getCurtime(void);
uint32_t Time_getRealCurtime(void);
void Time_setTickrate(uint32_t tickrate);
uint32_t Time_getTickrate(void);
#if VARIABLE_TICKRATE == 1
uint32_t Time_millisecondsToTicks(uint32_t ms);
uint32_t Time_secondsToTicks(uint32_t sec);
#endif
uint32_t Time_microseconds(void);
void Time_delayMicroseconds(uint32_t us);
void Time_delayMilliseconds(uint32_t ms);

#ifdef VORTEX_LIB
uint32_t Time_startSimulation(void);
uint32_t Time_tickSimulation(void);
bool Time_isSimulation(void);
uint32_t Time_getSimulationTick(void);
uint32_t Time_endSimulation(void);
void Time_setInstantTimestep(bool instant);
bool Time_isInstantStepping(void);
#endif

#if TIMER_TEST == 1
void Time_test(void);
#endif

extern uint32_t Time_curTick;
#if VARIABLE_TICKRATE == 1
extern uint32_t Time_tickrate;
#endif
#ifdef VORTEX_LIB
extern uint32_t Time_prevTime;
extern uint32_t Time_firstTime;
extern uint32_t Time_simulationTick;
extern bool Time_isSimulation_;
extern bool Time_instantTimestep;
#endif
#ifdef VORTEX_EMBEDDED
void Time_initMCUTime(void);
#endif

#endif
