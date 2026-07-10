#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>
#include <stdbool.h>

#include "../Leds/LedTypes.h"

#define ALARM_NONE -1

typedef int8_t AlarmID;

#define TIMER_FLAGS_NONE  0

#define TIMER_1_ALARM     1
#define TIMER_2_ALARMS    2
#define TIMER_3_ALARMS    3
#define TIMER_4_ALARMS    4

#define TIMER_ALARM_MASK  ( 1 | 2 | 3 | 4 )

#define TIMER_START       (1 << 7)

typedef struct Timer_s {
  uint32_t *alarms;
  uint8_t numAlarms;
  AlarmID curAlarm;
  uint32_t startTime;
#ifdef VORTEX_LIB
  uint32_t simStartTime;
#endif
} Timer;

void Timer_init(Timer *self);
void Timer_cleanup(Timer *self);
void Timer_initFlags(Timer *self, uint8_t flags, uint8_t alarm1, uint8_t alarm2, uint8_t alarm3, uint8_t alarm4);
AlarmID Timer_addAlarm(Timer *self, uint32_t interval);
void Timer_restart(Timer *self, uint32_t offset);
void Timer_start(Timer *self, uint32_t offset);
void Timer_reset(Timer *self);
bool Timer_onStart(const Timer *self);
bool Timer_onEnd(const Timer *self);
AlarmID Timer_alarm(Timer *self);
uint32_t Timer_numAlarms(const Timer *self);
uint32_t Timer_curAlarm(const Timer *self);
uint32_t Timer_startTime(const Timer *self);

#if TIMER_TEST == 1
void Timer_test(void);
#endif

#endif
