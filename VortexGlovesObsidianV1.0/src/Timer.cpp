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

bool Timer::addAlarm(uint32_t interval)
{
  void *temp = realloc(m_alarms, sizeof(uint32_t) * (m_numAlarms + 1));
  if (!temp) {
    return false;
  }
  m_alarms = (uint32_t *)temp;
  m_alarms[m_numAlarms] = interval;
  m_totalTime += interval;
  m_numAlarms++;
  return true;
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

AlarmID Timer::alarm(int32_t *numTriggers)
{
  if (!m_numAlarms || !m_alarms || m_curAlarm == ALARM_NONE) {
    return ALARM_NONE;
  }

  // grab curTime
  uint64_t now = Time::getCurtime();

  // timers use a different 'start time' tracker in simulations
  // this allows them to rollback seamlessly when the simulation ends
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

  // if it's the first tick since starting the alarm, and the alarmtime isn't 0 then just 
  // return 0 becuase the modulus below will always evaluate to 0 if timeDiff is 0
  if (timeDiff == 0 && alarmTime != 0) {
    //return ALARM_NONE;
  }

  // if the current alarm duration has passed on this tick
  // use modulus so that the timer alarms work in reverse too
  if ((timeDiff % alarmTime) == 0) {
    if (numTriggers) {
      int32_t triggers = timeDiff / (int32_t)m_totalTime; 
      // the number of times the alarm would have triggered
      *numTriggers = triggers;
    }
    if (Time::isSimulation()) {
      // move the sim start time
      m_simStartTime = now;
    } else {
      // move the start time forward
      m_startTime = now;
    }
    // store the alarm ID that will be returned
    AlarmID curAlarm = m_curAlarm;
    // increment to the next timer
    m_curAlarm = (AlarmID)((m_curAlarm + 1) % m_numAlarms);
    // and hit the timer
    return curAlarm;
  } 
  return ALARM_NONE;
}
