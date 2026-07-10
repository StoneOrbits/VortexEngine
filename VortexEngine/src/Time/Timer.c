#include "Timer.h"

#include "TimeControl.h"

#include "../Memory/Memory.h"
#include "../Log/Log.h"

static uint32_t Timer_getStartTime(const Timer *self)
{
#ifdef VORTEX_LIB
  if (Time_isSimulation() && self->simStartTime) {
    return self->simStartTime;
  }
#endif
  return self->startTime;
}

static void Timer_setStartTime(Timer *self, uint32_t tick)
{
#ifdef VORTEX_LIB
  if (Time_isSimulation()) {
    self->simStartTime = tick;
    return;
  }
#endif
  self->startTime = tick;
}

void Timer_init(Timer *self)
{
  self->alarms = NULL;
  self->numAlarms = 0;
  self->curAlarm = 0;
  self->startTime = 0;
#ifdef VORTEX_LIB
  self->simStartTime = 0;
#endif
}

void Timer_cleanup(Timer *self)
{
  if (self->alarms) {
    vfree(self->alarms);
    self->alarms = NULL;
  }
}

void Timer_initFlags(Timer *self, uint8_t flags, uint8_t alarm1, uint8_t alarm2, uint8_t alarm3, uint8_t alarm4)
{
  Timer_reset(self);
  if ((flags & TIMER_ALARM_MASK) >= TIMER_1_ALARM) Timer_addAlarm(self, alarm1);
  if ((flags & TIMER_ALARM_MASK) >= TIMER_2_ALARMS) Timer_addAlarm(self, alarm2);
  if ((flags & TIMER_ALARM_MASK) >= TIMER_3_ALARMS) Timer_addAlarm(self, alarm3);
  if ((flags & TIMER_ALARM_MASK) >= TIMER_4_ALARMS) Timer_addAlarm(self, alarm4);
  if ((flags & TIMER_START) != 0) Timer_start(self, 0);
}

AlarmID Timer_addAlarm(Timer *self, uint32_t interval)
{
  void *temp = vrealloc(self->alarms, sizeof(uint32_t) * (self->numAlarms + 1));
  if (!temp) {
    ERROR_OUT_OF_MEMORY();
    return -1;
  }
  self->alarms = (uint32_t *)temp;
  self->alarms[self->numAlarms] = interval;
  return (AlarmID)(self->numAlarms++);
}

void Timer_restart(Timer *self, uint32_t offset)
{
  Timer_start(self, offset);
  self->curAlarm = 0;
}

void Timer_start(Timer *self, uint32_t offset)
{
  self->startTime = Time_getCurtime() + offset;
#ifdef VORTEX_LIB
  self->simStartTime = self->startTime;
#endif
}

void Timer_reset(Timer *self)
{
  vfree(self->alarms);
  self->alarms = NULL;
  self->numAlarms = 0;
  self->curAlarm = 0;
  self->startTime = 0;
#ifdef VORTEX_LIB
  self->simStartTime = 0;
#endif
}

bool Timer_onStart(const Timer *self)
{
  if (Time_getCurtime() == Timer_getStartTime(self) && self->curAlarm == 0) {
    return true;
  }
  return false;
}

bool Timer_onEnd(const Timer *self)
{
  if (!self->alarms || !self->numAlarms) {
    return false;
  }
  uint32_t alarmTime = self->alarms[self->curAlarm];
  if (alarmTime <= 1) {
    return true;
  }
  uint32_t now = Time_getCurtime();
  uint32_t startTime = Timer_getStartTime(self);
  int32_t timeDiff = (int32_t)(int64_t)(now - startTime);
  if (!timeDiff) {
    return false;
  }
  return ((timeDiff % (alarmTime - 1)) == 0);
}

AlarmID Timer_alarm(Timer *self)
{
  if (!self->numAlarms || !self->alarms || self->curAlarm == ALARM_NONE) {
    return ALARM_NONE;
  }
  uint32_t now = Time_getCurtime();
  int32_t timeDiff = (int32_t)(int64_t)(now - Timer_getStartTime(self));
  if (timeDiff < 0) {
    return ALARM_NONE;
  }
  if (timeDiff == 0) {
    return 0;
  }
  uint32_t alarmTime = self->alarms[self->curAlarm];
  if (alarmTime && (timeDiff % alarmTime) != 0) {
    return ALARM_NONE;
  }
  Timer_setStartTime(self, now);
  self->curAlarm = (AlarmID)((self->curAlarm + 1) % self->numAlarms);
  return self->curAlarm;
}

uint32_t Timer_numAlarms(const Timer *self)
{
  return (uint32_t)self->numAlarms;
}

uint32_t Timer_curAlarm(const Timer *self)
{
  return (uint32_t)self->curAlarm;
}

uint32_t Timer_startTime(const Timer *self)
{
  return self->startTime;
}

#if TIMER_TEST == 1
#include <assert.h>

void Timer_test(void)
{
  DEBUG_LOG("Starting Timer class tests...");
  Timer newTimer;
  Timer_init(&newTimer);

  AlarmID alarm0 = Timer_addAlarm(&newTimer, 5);
  AlarmID alarm1 = Timer_addAlarm(&newTimer, 10);
  AlarmID alarm2 = Timer_addAlarm(&newTimer, 20);
  assert(alarm0 == 0);
  assert(alarm1 == 1);
  assert(alarm2 == 2);
  DEBUG_LOG("Alarm addition tests passed.");

  Timer_start(&newTimer, 0);
  assert(Timer_alarm(&newTimer) == alarm0);
  DEBUG_LOG("Alarm start test passed.");

  for (uint32_t t = 0; t <= 35; ++t) {
    AlarmID alarm = Timer_alarm(&newTimer);
    if (t == 0) {
      assert(alarm == alarm0);
    } else if (t == 5) {
      assert(alarm == alarm1);
    } else if (t == 15) {
      assert(alarm == alarm2);
    } else if (t == 35) {
      assert(alarm == alarm0);
    } else {
      assert(alarm == ALARM_NONE);
    }
    Time_tickClock();
  }

#ifdef VORTEX_LIB
  Timer_init(&newTimer);

  AlarmID simAlarm0 = Timer_addAlarm(&newTimer, 5);
  AlarmID simAlarm1 = Timer_addAlarm(&newTimer, 10);
  AlarmID simAlarm2 = Timer_addAlarm(&newTimer, 20);

  Timer_start(&newTimer, 0);
  assert(Timer_alarm(&newTimer) == simAlarm0);

  Time_startSimulation();
  for (uint32_t t = 0; t <= 35; ++t) {
    AlarmID alarm = Timer_alarm(&newTimer);
    if (t == 0) {
      assert(alarm == simAlarm0);
    } else if (t == 5) {
      assert(alarm == simAlarm1);
    } else if (t == 15) {
      assert(alarm == simAlarm2);
    } else if (t == 35) {
      assert(alarm == simAlarm0);
    } else {
      assert(alarm == ALARM_NONE);
    }
    Time_tickSimulation();
  }
  Time_endSimulation();
#endif

  DEBUG_LOG("Timer class tests completed successfully.");
}
#endif
