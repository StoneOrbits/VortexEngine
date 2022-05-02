#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

#include "LedConfig.h"

#define ALARM_NONE -1

// an alarm id is just an int
typedef int32_t AlarmID;

class Timer
{
public:
  Timer();
  
  // add alarms to the timer and returns it's ID
  // Alarm IDs start at 0 and count upward
  AlarmID addAlarm(uint32_t interval);

  // restart the timer entirely
  void restart();

  // start the timer but don't change current alarm, this shifts 
  // the timer startTime but does not reset it's alarm state
  void start();

  // delete all alarms from the timer and reset
  void reset();

  // Will return the alarmID of the alarm that triggered
  // If no alarm hits it will return ALARM_NONE.
  // Outputs an integer indicating the number of times the alarm has triggered
  // since the start time or the last detection of that alarm ID
  AlarmID alarm();

  // the number of alarms
  uint32_t numAlarms() const { return m_numAlarms; }

private:
  // the list of alarms and number of alarms
  uint32_t *m_alarms; 
  uint32_t m_numAlarms;

  // the current alarm being checked
  AlarmID m_curAlarm;

  // start time in microseconds
  uint64_t m_startTime;

  // the simulation start time
  uint64_t m_simStartTime;

  // the total time the timer covers (sum of all alarms)
  uint64_t m_totalTime;
};

#endif
