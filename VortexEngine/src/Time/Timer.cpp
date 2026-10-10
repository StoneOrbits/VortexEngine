#include "Timer.h"

#include "TimeControl.h"

#include "../Memory/Memory.h"
#include "../Log/Log.h"

void Timer_make(Timer *self)
{
  self->m_numAlarms = 0;
  self->m_curAlarm = 0;
  self->m_startTime = 0;
#ifdef VORTEX_LIB
  self->m_simStartTime = 0;
#endif
}

void Timer_destroy(Timer *self)
{
  // the alarms are stored inline, nothing to release
  (void)self;
}

void Timer_init5(Timer *self, uint8_t flags, uint8_t alarm1, uint8_t alarm2, uint8_t alarm3, uint8_t alarm4)
{
  Timer_reset(self);
  if ((flags & TIMER_ALARM_MASK) >= TIMER_1_ALARM) Timer_addAlarm1(self, alarm1);
  if ((flags & TIMER_ALARM_MASK) >= TIMER_2_ALARMS) Timer_addAlarm1(self, alarm2);
  if ((flags & TIMER_ALARM_MASK) >= TIMER_3_ALARMS) Timer_addAlarm1(self, alarm3);
  if ((flags & TIMER_ALARM_MASK) >= TIMER_4_ALARMS) Timer_addAlarm1(self, alarm4);
  if ((flags & TIMER_START) != 0) Timer_start0(self);
}


// arity shims for the optional trailing arguments
void Timer_init1(Timer *self, uint8_t flags) { Timer_init5(self, flags, 0, 0, 0, 0); }
void Timer_init2(Timer *self, uint8_t flags, uint8_t alarm1) { Timer_init5(self, flags, alarm1, 0, 0, 0); }
void Timer_init3(Timer *self, uint8_t flags, uint8_t alarm1, uint8_t alarm2) { Timer_init5(self, flags, alarm1, alarm2, 0, 0); }
void Timer_init4(Timer *self, uint8_t flags, uint8_t alarm1, uint8_t alarm2, uint8_t alarm3) { Timer_init5(self, flags, alarm1, alarm2, alarm3, 0); }
void Timer_restart0(Timer *self) { Timer_restart(self, 0); }
void Timer_start0(Timer *self) { Timer_start(self, 0); }

AlarmID Timer_addAlarm1(Timer *self, uint32_t interval)
{
  if (self->m_numAlarms >= TIMER_MAX_ALARMS) {
    ERROR_LOG("Too many alarms");
    return ALARM_NONE;
  }
  self->m_alarms[self->m_numAlarms] = interval;
  return self->m_numAlarms++;
}

void Timer_restart(Timer *self, uint32_t offset)
{
  Timer_start(self, offset);
  // reset the current alarm
  self->m_curAlarm = 0;
}

void Timer_start(Timer *self, uint32_t offset)
{
  // reset the start time
  self->m_startTime = Time_getCurtime() + offset;
#ifdef VORTEX_LIB
  self->m_simStartTime = self->m_startTime;
#endif
}

void Timer_reset(Timer *self)
{
  self->m_numAlarms = 0;
  self->m_curAlarm = 0;
  self->m_startTime = 0;
#ifdef VORTEX_LIB
  self->m_simStartTime = 0;
#endif
}

bool Timer_onStart(const Timer *self)
{
  if (Time_getCurtime() == Timer_getStartTime(self) && self->m_curAlarm == 0) {
    return true;
  }
  return false;
}

bool Timer_onEnd(const Timer *self)
{
  if (!self->m_numAlarms) {
    return false;
  }
  // the tick for when this alarm triggers
  uint32_t alarmTime = self->m_alarms[self->m_curAlarm];
  if (alarmTime <= 1) {
    return true;
  }
  // grab curTime
  uint32_t now = Time_getCurtime();
  // get the start time of the timer
  uint32_t startTime = Timer_getStartTime(self);
  // time since start (forward or backwards)
  int32_t timeDiff = (int32_t)(int64_t)(now - startTime);
  // if no time since start then this definitely isn't the end
  if (!timeDiff) {
    return false;
  }
  // check if timediff is multiple of last tick in alarm
  return ((timeDiff % (alarmTime - 1)) == 0);
}

AlarmID Timer_alarm(Timer *self)
{
  if (!self->m_numAlarms || self->m_curAlarm == ALARM_NONE) {
    return ALARM_NONE;
  }
  uint32_t now = Time_getCurtime();
  // time since start (forward or backwards)
  int32_t timeDiff = (int32_t)(int64_t)(now - Timer_getStartTime(self));
  if (timeDiff < 0) {
    return ALARM_NONE;
  }
  // if no time passed it's first alarm that is starting
  if (timeDiff == 0) {
    return 0;
  }
  uint32_t alarmTime = self->m_alarms[self->m_curAlarm];
  // if the current alarm duration is not a multiple of the current tick
  if (alarmTime && (timeDiff % alarmTime) != 0) {
    // then the alarm was not hit
    return ALARM_NONE;
  }
  // update the start time of the timer
  Timer_setStartTime(self, now);
  // increment current alarm then return that id
  self->m_curAlarm = (AlarmID)((self->m_curAlarm + 1) % self->m_numAlarms);
  return self->m_curAlarm;
}

uint32_t Timer_getStartTime(const Timer *self)
{
#ifdef VORTEX_LIB
  // timers use a different 'start time' tracker in simulations so that
  // the startTime remains unchanged when the simulation ends
  if (Time_isSimulation() && self->m_simStartTime) {
    return self->m_simStartTime;
  }
#endif
  return self->m_startTime;
}

void Timer_setStartTime(Timer *self, uint32_t tick)
{
#ifdef VORTEX_LIB
  // if this timer is running in a simulation then don't actually update
  // the starttime, instead update the simulation start time so that when
  // the simulation ends the startTime will not have changed at all
  if (Time_isSimulation()) {
    // move the sim start time
    self->m_simStartTime = tick;
    return;
  }
#endif
  // move the start time forward
  self->m_startTime = tick;
}

#if TIMER_TEST == 1
#include <assert.h>

void Timer_test(void)
{
  DEBUG_LOG("Starting Timer class tests...");
  Timer newTimer;
  Timer_make(&newTimer);

  // Test: Add alarms and check if alarms are added correctly
  Timer_init1(&newTimer, TIMER_FLAGS_NONE);
  AlarmID alarm0 = Timer_addAlarm(&newTimer, 5);
  AlarmID alarm1 = Timer_addAlarm(&newTimer, 10);
  AlarmID alarm2 = Timer_addAlarm(&newTimer, 20);
  assert(alarm0 == 0);
  assert(alarm1 == 1);
  assert(alarm2 == 2);
  DEBUG_LOG("Alarm addition tests passed.");

  // Test: Start timer and check for alarms
  Timer_start0(&newTimer);
  // alarm1 should trigger immediately because it is starting
  assert(Timer_alarm(&newTimer) == alarm0);
  DEBUG_LOG("Alarm start test passed.");

  // Test: Advance time and check for alarms
  for (uint32_t t = 0; t <= 35; ++t) {
    AlarmID alarm = Timer_alarm(&newTimer);
    if (t == 0) { // 0th tick timer 0 fires as it begins
      assert(alarm == alarm0);
      DEBUG_LOG("Alarm 1 triggered as expected.");
    } else if (t == 5) { // 6th tick timer 1 fires after 5 ticks of timer 0
      assert(alarm == alarm1);
      DEBUG_LOG("Alarm 2 triggered as expected.");
    } else if (t == 15) { // 16th tick timer 2 fires after 15 ticks of timer 0 and 1
      assert(alarm == alarm2);
      DEBUG_LOG("Alarm 3 triggered as expected.");
    } else if (t == 35) { // 36th tick timer 0 fires against after 35 ticks of 0 1 2
      assert(alarm == alarm0);
      DEBUG_LOG("Alarm 1 triggered as expected.");
    } else {
      assert(alarm == ALARM_NONE);
    }
    Time_tickClock();
  }

#ifdef VORTEX_LIB
  // Test time simulation APIs in vortex lib builds
  Timer_init1(&newTimer, TIMER_FLAGS_NONE);

  // Add alarms
  AlarmID simAlarm0 = Timer_addAlarm(&newTimer, 5);
  AlarmID simAlarm1 = Timer_addAlarm(&newTimer, 10);
  AlarmID simAlarm2 = Timer_addAlarm(&newTimer, 20);

  Timer_start0(&newTimer);
  assert(Timer_alarm(&newTimer) == simAlarm0);

  Time_startSimulation();
  for (uint32_t t = 0; t <= 35; ++t) {
    AlarmID alarm = Timer_alarm(&newTimer);
    if (t == 0) { // 0th tick timer 0 fires as it begins
      assert(alarm == simAlarm0);
      DEBUG_LOG("Alarm 1 triggered as expected.");
    } else if (t == 5) { // 6th tick timer 1 fires after 5 ticks of timer 0
      assert(alarm == simAlarm1);
      DEBUG_LOG("Alarm 2 triggered as expected.");
    } else if (t == 15) { // 16th tick timer 2 fires after 15 ticks of timer 0 and 1
      assert(alarm == simAlarm2);
      DEBUG_LOG("Alarm 3 triggered as expected.");
    } else if (t == 35) { // 36th tick timer 0 fires against after 35 ticks of 0 1 2
      assert(alarm == simAlarm0);
      DEBUG_LOG("Alarm 1 triggered as expected.");
    } else {
      assert(alarm == ALARM_NONE);
    }
    Time_tickSimulation();
  }
  Time_endSimulation();
  DEBUG_LOG("Time simulation tests passed.");
#endif

  DEBUG_LOG("Timer class tests completed successfully.");
}
#endif

AlarmID Timer_addAlarm(Timer *self, uint32_t interval)
{
  return Timer_addAlarm1(self, interval);
}
