#ifndef TIME_CONTROL_H
#define TIME_CONTROL_H

#include <inttypes.h>

#include "LedTypes.h"

class Time
{
  // private unimplemented constructor
  Time();

public:
  // opting for static class here because there should only ever be one
  // Settings control object and I don't like singletons
  static bool init();
  static void cleanup();

  // tick the clock forward to millis()
  static void tickClock();

  // get the current time with optional led position time offset
  static uint64_t getCurtime(LedPos pos = LED_FIRST);

  // this ignore simulation time, it's used by timers to make simulations work
  static uint64_t getRealCurtime();

  // get the amount of ticks this led position runs out of sync
  // the 1st index led gets exactly 1x the tick offset
  static uint32_t getTickOffset(LedPos pos = (LedPos)1);

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

  // The current tickrate
  static uint32_t getTickrate() { return m_tickrate; }

  // change the number of ticks each LED runs out of sync
  // 0 will run all of the lights in sync
  static void setTickOffset(uint32_t tickOffset = 0);

  // convert milliseconds to a tickcount based on tickrate
  static uint32_t msToTicks(uint32_t ms);

  // convert seconds to a tickcount based on tickrate
  static uint32_t secToTicks(uint32_t sec) { return msToTicks(sec * 1000); }

  // Start a time simulation, while the simulation is active you can
  // increment the 'current time' with tickSimulation() and all calls
  // to Time::getCurtime will reflect the changes, then when you call
  // endSimulation() the currentTime will be restored
  static uint32_t startSimulation();

  // Tick a time simulation forward, returning the next tick
  static uint32_t tickSimulation();

  // Whether running a time simulation
  static bool isSimulation();

  // get the current tick in the simulation
  static uint32_t getSimulationTick();

  // Finish a time simulation
  static uint32_t endSimulation();

private:
  // global tick counter
  static uint64_t m_curTick;

  // the last frame timestamp
  static uint64_t m_prevTime;

  // the first timestamp
  static uint64_t m_firstTime;

  // the number of ticks per second
  static uint32_t m_tickrate;

  // the offset in ticks for each finger
  static uint32_t m_tickOffset;

  // the current simulation offset, simulations are
  // used to fastforward patterns and colorsets by
  // simulating tick changes and running pattern logic
  // multiple times in a single frame
  static uint32_t m_simulationTick;

  // whether the timer is running a simulation
  static bool m_isSimulation;
};

#endif
