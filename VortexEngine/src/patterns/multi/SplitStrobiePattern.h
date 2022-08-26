#ifndef SPLITSTROBIE_PATTERN_H
#define SPLITSTROBIE_PATTERN_H

#include "HybridPattern.h"
#include "../../Timer.h"

class SplitStrobiePattern : public HybridPattern
{
public:
  SplitStrobiePattern();
  virtual ~SplitStrobiePattern();

  // init the pattern to initial state
  virtual void init() override;
  virtual void play() override;

private:
  // the step timer
  Timer m_stepTimer;

  bool m_switch;
};

#endif 
