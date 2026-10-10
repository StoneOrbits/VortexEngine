#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>
#include <stdbool.h>

#include "../Leds/LedTypes.h"
#include "../VortexConfig.h"

#define ALARM_NONE -1

// an alarm id is just an int
typedef int8_t AlarmID;

#define TIMER_FLAGS_NONE  0

// the number of alarms
#define TIMER_1_ALARM     1
#define TIMER_2_ALARMS    2
#define TIMER_3_ALARMS    3
#define TIMER_4_ALARMS    4

// bitmask for all alarms
#define TIMER_ALARM_MASK  ( 1 | 2 | 3 | 4 )

// start the alarm
#define TIMER_START       (1 << 7)

// the maximum number of alarms a timer can hold, the alarms are stored
// inline in the timer so that adding alarms never touches the heap. The
// slim build only uses single alarm timers (basic patterns), the full build
// needs enough for the multi-led patterns and their sequence steps
#if VORTEX_SLIM == 1
#define TIMER_MAX_ALARMS 1
#else
#define TIMER_MAX_ALARMS ((LED_COUNT > 4) ? LED_COUNT : 4)
#endif

typedef struct Timer {
  // the list of alarms and number of alarms
  uint32_t m_alarms[TIMER_MAX_ALARMS];
  uint8_t m_numAlarms;
  // the current alarm being checked
  AlarmID m_curAlarm;
  // start time in microseconds
  uint32_t m_startTime;

  // only need time simulations for test framework pattern strip generation
#ifdef VORTEX_LIB
  // the simulation start time
  uint32_t m_simStartTime;
#endif
} Timer;

void Timer_make(Timer *self);
void Timer_destroy(Timer *self);

// init a timer with a number of alarms and optionally start it
void Timer_init(Timer *self, uint8_t flags, uint8_t alarm1, uint8_t alarm2, uint8_t alarm3, uint8_t alarm4);
void Timer_init1(Timer *self, uint8_t flags);
void Timer_init2(Timer *self, uint8_t flags, uint8_t alarm1);
void Timer_init3(Timer *self, uint8_t flags, uint8_t alarm1, uint8_t alarm2);
void Timer_init4(Timer *self, uint8_t flags, uint8_t alarm1, uint8_t alarm2, uint8_t alarm3);
void Timer_init5(Timer *self, uint8_t flags, uint8_t alarm1, uint8_t alarm2, uint8_t alarm3, uint8_t alarm4);

#define TIMER_NARG(_1, _2, _3, _4, _5, N, ...) N
#define TIMER_COUNT(...) TIMER_NARG(__VA_ARGS__, 5, 4, 3, 2, 1)
#define TIMER_CAT_(a, b) a##b
#define TIMER_INIT_(N, ...) TIMER_CAT_(Timer_init, N)(__VA_ARGS__)

#define Timer_initN(self, ...) TIMER_INIT_(TIMER_COUNT(__VA_ARGS__), self, __VA_ARGS__)

// add alarms to the timer and returns it's ID
// Alarm IDs start at 0 and count upward
AlarmID Timer_addAlarm(Timer *self, uint32_t interval);
AlarmID Timer_addAlarm1(Timer *self, uint32_t interval);

// helpers to set/get start time
uint32_t Timer_getStartTime(const Timer *self);
void Timer_setStartTime(Timer *self, uint32_t tick);

// restart the timer entirely
void Timer_restart(Timer *self, uint32_t offset);
void Timer_restart0(Timer *self);

// start the timer but don't change current alarm, this shifts
// the timer startTime but does not reset it's alarm state
void Timer_start(Timer *self, uint32_t offset);
void Timer_start0(Timer *self);

#define TIMER_OFF_(fn, N, ...) TIMER_CAT_(fn, N)(__VA_ARGS__)
#define Timer_restartN(self, ...) TIMER_OFF_(Timer_restart, TIMER_COUNT(__VA_ARGS__), self, __VA_ARGS__)
#define Timer_startN(self, ...) TIMER_OFF_(Timer_start, TIMER_COUNT(__VA_ARGS__), self, __VA_ARGS__)

// delete all alarms from the timer and reset
void Timer_reset(Timer *self);

// returns true when the current timer is starting, used for synchronization
bool Timer_onStart(const Timer *self);
// returns true when the current timer is ending, used for synchronization
bool Timer_onEnd(const Timer *self);

// Will return the alarmID of the alarm that triggered
// If no alarm hits it will return ALARM_NONE.
// Outputs an integer indicating the number of times the alarm has triggered
// since the start time or the last detection of that alarm ID
AlarmID Timer_alarm(Timer *self);

// the number of alarms
static inline uint32_t Timer_numAlarms(const Timer *self) { return self->m_numAlarms; }
static inline uint32_t Timer_curAlarm(const Timer *self) { return self->m_curAlarm; }

// the start time of the timer
static inline uint32_t Timer_startTime(const Timer *self) { return self->m_startTime; }

#if TIMER_TEST == 1
void Timer_test(void);
#endif

#endif
