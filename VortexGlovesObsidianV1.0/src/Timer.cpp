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

void Timer::start(uint32_t offset)
{
  // reset the start time
  m_startTime = Time::getCurtime() + offset;
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

bool Timer::onStart() const
{
  if (Time::getCurtime() == getStartTime() && m_curAlarm == 0) {
    DEBUG("Timer start");
    return true;
  }
  return false;
}

bool Timer::onEnd() const
{
  if (!m_alarms || !m_numAlarms) {
    return false;
  }
  // the tick for when this alarm triggers
  uint32_t alarmTime = m_alarms[m_curAlarm];
  if (alarmTime <= 1) {
    return true;
  }
  // grab curTime
  uint64_t now = Time::getCurtime();
  // get the start time of the timer
  uint64_t startTime = getStartTime();
  // time since start (forward or backwards)
  int32_t timeDiff = now - startTime;
  return ((timeDiff % (alarmTime - 1)) == 0);
}

AlarmID Timer::alarm()
{
  if (!m_numAlarms || !m_alarms || m_curAlarm == ALARM_NONE) {
    return ALARM_NONE;
  }

  // grab curTime
  uint64_t now = Time::getCurtime();

  // get the start time of the timer
  uint64_t startTime = getStartTime();

  // time since start (forward or backwards)
  int32_t timeDiff = now - startTime;
  if (timeDiff < 0) {
    return ALARM_NONE;
  }

  // the tick for when this alarm triggers
  uint32_t alarmTime = m_alarms[m_curAlarm];

  // if the current alarm duration is not a multiple of the current tick
  if (alarmTime && (timeDiff % alarmTime) != 0) {
    // then the alarm was not hit
    return ALARM_NONE;
  }

  // update the start time of the timer
  setStartTime(now);

  // store the alarm ID that will be returned
  AlarmID curAlarm = m_curAlarm;
  // increment to the next alarm
  m_curAlarm = (AlarmID)((m_curAlarm + 1) % m_numAlarms);

  // return the alarm id
  return curAlarm;
}

uint64_t Timer::getStartTime() const
{
  // timers use a different 'start time' tracker in simulations so that
  // the startTime remains unchanged when the simulation ends
  if (Time::isSimulation() && m_simStartTime) {
    return m_simStartTime;
  }
  return m_startTime;
}

void Timer::setStartTime(uint64_t tick)
{
  // if this timer is running in a simulation then don't actually update
  // the starttime, instead update the simulation start time so that when
  // the simulation ends the startTime will not have changed at all
  if (Time::isSimulation()) {
    // move the sim start time
    m_simStartTime = tick;
  } else {
    // move the start time forward
    m_startTime = tick;
  }
}
