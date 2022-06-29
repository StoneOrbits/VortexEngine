#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

#include "LedTypes.h"

#define ALARM_NONE -1

// an alarm id is just an int
typedef int8_t AlarmID;

class Timer
{
public:
  Timer();
  ~Timer();

  // add alarms to the timer and returns it's ID
  // Alarm IDs start at 0 and count upward
  AlarmID addAlarm(uint32_t interval);

  // restart the timer entirely
  void restart(uint32_t offset = 0);

  // start the timer but don't change current alarm, this shifts
  // the timer startTime but does not reset it's alarm state
  void start(uint32_t offset = 0);

  // delete all alarms from the timer and reset
  void reset();

  // returns true when the current timer is starting, used for synchronization
  bool onStart() const;
  // returns true when the current timer is ending, used for synchronization
  bool onEnd() const;

  // Will return the alarmID of the alarm that triggered
  // If no alarm hits it will return ALARM_NONE.
  // Outputs an integer indicating the number of times the alarm has triggered
  // since the start time or the last detection of that alarm ID
  AlarmID alarm();

  // the number of alarms
  uint32_t numAlarms() const { return m_numAlarms; }
  uint32_t curAlarm() const { return m_curAlarm; }

  // the start time of the timer
  uint64_t startTime() const { return m_startTime; }

private:
  // helpers to set/get start time
  uint64_t getStartTime() const;
  void setStartTime(uint64_t);

  // the list of alarms and number of alarms
  uint32_t *m_alarms;
  uint8_t m_numAlarms;
  // the current alarm being checked
  AlarmID m_curAlarm;
  // start time in microseconds
  uint64_t m_startTime;
  // the simulation start time
  uint64_t m_simStartTime;
};

#endif
