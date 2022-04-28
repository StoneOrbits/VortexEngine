#ifndef TIME_H
#define TIME_H

#include <inttypes.h>

#include "LedConfig.h"

class Time
{
public:
  // opting for static class here because there should only ever be one
  // Settings control object and I don't like singletons
  static bool init();

  // tick the clock forward to millis()
  static void tickClock();

  // get the current time with optional led position time offset
  static uint64_t getCurtime(LedPos pos = LED_FIRST);

  // Set tickrate in Ticks Per Second (TPS)
  // The valid range for this is 1 <= x <= 1000000
  //
  // Setting a value of 0 will restore the default
  //
  // NOTE: Patterns will not change when this changes, you
  //       must re-create the pattern for the change to take
  //       effect. This is done by design to allow the test
  //       framework to control the speed of patterns.
  //       See PatternBuilder.cpp for more info.
  static void setTickrate(uint32_t tickrate = 0);

  // change the number of ticks each LED runs out of sync
  // 0 will run all of the lights in sync
  // TODO: Synchronize finger timing?
  static void setTickOffset(uint32_t tickOffset = 0);

  // convert ticks to ms based on tickrate
  static uint32_t msToTicks(uint32_t ms);

private:
  // global tick counter
  static uint64_t m_curTick;

  // the last frame timestamp
  static uint64_t m_prevTime;

  // the number of ticks per second
  static uint32_t m_tickrate;

  // the offset in ticks for each finger
  static uint32_t m_tickOffset;
};

#endif
