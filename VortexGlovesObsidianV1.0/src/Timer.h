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
  
  // add alarms to the timer
  bool addAlarm(uint32_t interval);

  // restart the timer and the current alarm
  void restart(uint32_t offset = 0);

  // start the timer but don't touch current alarm
  void start(uint32_t offset = 0);

  // reset all alarms in the timer to 0
  void reset();

  // Will return the alarmID every interval ticks offset from start time
  // If no alarm hits it will return ALARM_NONE.
  // Outputs an integer indicating the number of times the alarm has triggered
  // since the start time or the last detection of that alarm ID
  AlarmID alarm(int32_t *numTriggers = nullptr);

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
