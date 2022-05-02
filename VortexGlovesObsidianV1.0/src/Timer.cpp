#include "Timer.h"

#include <stdlib.h>

#include "TimeControl.h"
#include "Log.h"

Timer::Timer() :
  m_alarms(nullptr),
  m_numAlarms(0),
  m_curAlarm(0),
  m_startTime(0),
  m_simStartTime(0),
  m_totalTime(0)
{
}

AlarmID Timer::addAlarm(uint32_t interval)
{
  void *temp = realloc(m_alarms, sizeof(uint32_t) * (m_numAlarms + 1));
  if (!temp) {
    return false;
  }
  m_alarms = (uint32_t *)temp;
  m_alarms[m_numAlarms] = interval;
  m_totalTime += interval;
  return m_numAlarms++;
}

void Timer::restart()
{
  start();
  // reset the current alarm
  m_curAlarm = 0;
}

void Timer::start()
{
  // reset the start time
  m_startTime = Time::getCurtime();
}

void Timer::reset()
{
  free(m_alarms);
  m_alarms = nullptr;
  m_numAlarms = 0;
  m_curAlarm = 0;
  m_startTime = 0;
  m_simStartTime = 0;
  m_totalTime = 0;
}

AlarmID Timer::alarm()
{
  if (!m_numAlarms || !m_alarms || m_curAlarm == ALARM_NONE) {
    return ALARM_NONE;
  }

  // grab curTime
  uint64_t now = Time::getCurtime();

  // timers use a different 'start time' tracker in simulations so that
  // the startTime remains unchanged when the simulation ends
  uint64_t startTime = 0;
  if (Time::isSimulation() && m_simStartTime) {
    startTime = m_simStartTime;
  } else {
    startTime = m_startTime;
  }

  // time since start (forward or backwards)
  int32_t timeDiff = now - startTime;
  // the tick for when this alarm triggers
  uint32_t alarmTime = m_alarms[m_curAlarm];

  // if the current alarm duration is not a multiple of the current tick
  if ((timeDiff % alarmTime) != 0) {
    // then the alarm was not hit
    return ALARM_NONE;
  }

  // if this timer is running in a simulation then don't actually update
  // the starttime, instead update the simulation start time so that when
  // the simulation ends the startTime will not have changed at all
  if (Time::isSimulation()) {
    // move the sim start time
    m_simStartTime = now;
  } else {
    // move the start time forward
    m_startTime = now;
  }

  // store the alarm ID that will be returned
  AlarmID curAlarm = m_curAlarm;
  // increment to the next alarm
  m_curAlarm = (AlarmID)((m_curAlarm + 1) % m_numAlarms);

  // return the alarm id
  return curAlarm;
}
