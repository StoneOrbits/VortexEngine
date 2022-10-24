#ifndef SPLITSTROBIE_PATTERN_H
#define SPLITSTROBIE_PATTERN_H

#include "HybridPattern.h"
#include "../../Timer.h"

class SplitStrobiePattern : public HybridPattern
{
public:
  SplitStrobiePattern(uint8_t stepDuration = 1000);
  virtual ~SplitStrobiePattern();

  // init the pattern to initial state
  virtual void init() override;
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:
  // the duration for the step
  uint8_t m_stepDuration;
  // the step timer
  Timer m_stepTimer;

  bool m_switch;
};

#endif 
