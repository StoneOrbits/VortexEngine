#include "Timer.h"

#include "TimeControl.h"

#include "../Memory/Memory.h"
#include "../Log/Log.h"

Timer::Timer() :
  m_alarms(nullptr),
  m_numAlarms(0),
  m_curAlarm(0),
  m_startTime(0)
#ifdef VORTEX_LIB
  , m_simStartTime(0)
#endif
{
}

Timer::~Timer()
{
  if (m_alarms) {
    vfree(m_alarms);
    m_alarms = nullptr;
  }
}

AlarmID Timer::addAlarm(uint32_t interval)
{
  void *temp = vrealloc(m_alarms, sizeof(uint32_t) * (m_numAlarms + 1));
  if (!temp) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  m_alarms = (uint32_t *)temp;
  m_alarms[m_numAlarms] = interval;
  return m_numAlarms++;
}

void Timer::restart(uint32_t offset)
{
  start(offset);
  // reset the current alarm
  m_curAlarm = 0;
}

void Timer::start(uint32_t offset)
{
  // reset the start time
  m_startTime = Time::getCurtime() + offset;
#ifdef VORTEX_LIB
  m_simStartTime = m_startTime;
#endif
}

void Timer::reset()
{
  vfree(m_alarms);
  m_alarms = nullptr;
  m_numAlarms = 0;
  m_curAlarm = 0;
  m_startTime = 0;
#ifdef VORTEX_LIB
  m_simStartTime = 0;
#endif
}

bool Timer::onStart() const
{
  if (Time::getCurtime() == getStartTime() && m_curAlarm == 0) {
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
  int32_t timeDiff = (int32_t)(int64_t)(now - startTime);
  // if no time since start then this definitely isn't the end
  if (!timeDiff) {
    return false;
  }
  // check if timediff is multiple of last tick in alarm
  return ((timeDiff % (alarmTime - 1)) == 0);
}

AlarmID Timer::alarm()
{
  if (!m_numAlarms || !m_alarms || m_curAlarm == ALARM_NONE) {
    return ALARM_NONE;
  }
  uint64_t now = Time::getCurtime();
  // time since start (forward or backwards)
  int32_t timeDiff = (int32_t)(int64_t)(now - getStartTime());
  if (timeDiff < 0) {
    return ALARM_NONE;
  }
  // if no time passed it's first alarm that is starting
  if (timeDiff == 0) {
    return 0;
  }
  uint32_t alarmTime = m_alarms[m_curAlarm];
  // if the current alarm duration is not a multiple of the current tick
  if (alarmTime && (timeDiff % alarmTime) != 0) {
    // then the alarm was not hit
    return ALARM_NONE;
  }
  // update the start time of the timer
  setStartTime(now);
  // increment current alarm then return that id
  m_curAlarm = (AlarmID)((m_curAlarm + 1) % m_numAlarms);
  return m_curAlarm;
}

uint64_t Timer::getStartTime() const
{
#ifdef VORTEX_LIB
  // timers use a different 'start time' tracker in simulations so that
  // the startTime remains unchanged when the simulation ends
  if (Time::isSimulation() && m_simStartTime) {
    return m_simStartTime;
  }
#endif
  return m_startTime;
}

void Timer::setStartTime(uint64_t tick)
{
#ifdef VORTEX_LIB
  // if this timer is running in a simulation then don't actually update
  // the starttime, instead update the simulation start time so that when
  // the simulation ends the startTime will not have changed at all
  if (Time::isSimulation()) {
    // move the sim start time
    m_simStartTime = tick;
    return;
  }
#endif
  // move the start time forward
  m_startTime = tick;
}
