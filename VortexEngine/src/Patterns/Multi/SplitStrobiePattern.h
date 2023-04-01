#ifndef SPLITSTROBIE_PATTERN_H
#define SPLITSTROBIE_PATTERN_H

#include "HybridPattern.h"

#include "../../Time/Timer.h"
#include "../../Time/Timings.h"

class SplitStrobiePattern : public HybridPattern
{
public:
  SplitStrobiePattern(const PatternArgs &args);
  virtual ~SplitStrobiePattern();

  // init the pattern to initial state
  virtual void init() override;
  virtual void play() override;

private:
  // the duration for the step in x100ms
  uint8_t m_stepDuration;
  // the step timer
  Timer m_stepTimer;

  bool m_switch;

  PatternArgs m_firstPatternArgs;
  PatternArgs m_secondPatternArgs;
};

#endif
