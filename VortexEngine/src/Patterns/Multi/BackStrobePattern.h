#ifndef BACKSTROBE_PATTERN_H
#define BACKSTROBE_PATTERN_H

#include "HybridPattern.h"

#include "../../Time/Timer.h"
#include "../../Time/Timings.h"

class BackStrobePattern : public HybridPattern
{
public:
  BackStrobePattern(uint8_t onDuration1 = 2, uint8_t offDuration1 = 4, uint8_t gapDuration1 = 0,
    uint8_t onDuration2 = HYPERSTROBE_ON_DURATION, uint8_t offDuration2 = HYPERSTROBE_OFF_DURATION, uint8_t gapDuration2 = 0, uint8_t stepSpeed100Ms = 10);
  BackStrobePattern(const PatternArgs &args);
  virtual ~BackStrobePattern();

  // init the pattern to initial state
  virtual void init() override;
  virtual void play() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

private:
  // the speed for the step timer in x100 ms chunks, so a value of 10
  // will be 1000ms
  uint8_t m_stepSpeed;
  // the step timer
  Timer m_stepTimer;

  bool m_switch;

  PatternArgs m_firstPatternArgs;
  PatternArgs m_secondPatternArgs;
};

#endif
