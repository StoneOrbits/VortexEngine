#ifndef TIME_H
#define TIME_H

#include <inttypes.h>

#include "LedConfig.h"

class TimeControl
{
  public:
    TimeControl();

    // any kind of time initialization
    bool init();

    // tick the clock forward to millis()
    void tickClock();

    // get the current time with optional led position time offset
    uint64_t getCurtime(LedPos pos = LED_FIRST) const;

  private:
    // global curtime
    uint64_t m_curTime;
};

// easy access to the time control
extern TimeControl *g_pTimeControl;

#endif
