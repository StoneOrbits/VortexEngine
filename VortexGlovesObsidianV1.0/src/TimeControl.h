#ifndef TIME_H
#define TIME_H

#include <inttypes.h>

#include "LedConfig.h"

// Tickrate in Ticks Per Second (TPS)
//
//    Default: 1000
//
// The valid range for this is 1 <= x <= 1000000
//
// However any value near or above 10000 will most
// likely be too fast for the processor to handle
#define DEFAULT_TICKRATE    1000

// Finger time offset in ticks
//
// This changes how many ticks out of sync each finger
// will run. So 33 means each finger runs 33 ticks out
// of sync with the previous finger
#define DEFAULT_TIME_OFFSET 33

// convert milliseconds to ticks
#define MS_TO_TICKS(ms) \
  (g_pTimeControl ? g_pTimeControl->msToTicks(ms) : (ms * DEFAULT_TICKRATE))

class TimeControl
{
public:
  TimeControl();
  ~TimeControl();

  // any kind of time initialization
  bool init();

  // tick the clock forward to millis()
  void tickClock();

  // get the current time with optional led position time offset
  uint64_t getCurtime(LedPos pos = LED_FIRST) const;

  // Set tickrate in Ticks Per Second (TPS)
  // The valid range for this is 1 <= x <= 1000000
  //
  // NOTE: Patterns will not change when this changes, you
  //       must re-create the pattern for the change to take
  //       effect. This is done by design to allow the test
  //       framework to control the speed of patterns.
  //       See PatternBuilder.cpp for more info.
  void setTickrate(uint32_t tickrate = DEFAULT_TICKRATE);

  // change the number of ticks each finger runs out of sync
  // 0 will run all of the lights in sync
  void setTimeOffset(uint32_t timeOffset = 0);

  // convert ticks to ms based on tickrate
  uint32_t msToTicks(uint32_t ms) const;

private:
  // global tick counter
  uint64_t m_curTick;

  // the last frame timestamp
  uint64_t m_prevTime;

  // the number of ticks per second
  uint32_t m_tickrate;

  // the offset in ticks for each finger
  uint32_t m_timeOffset;
};

// easy access to the time control
extern TimeControl *g_pTimeControl;

#endif
