#ifndef SPLITSTROBIE_PATTERN_H
#define SPLITSTROBIE_PATTERN_H

#include "HybridPattern.h"
#include "../../Timer.h"

class SplitStrobiePattern : public HybridPattern
{
public:
  SplitStrobiePattern(uint16_t stepDuration = 1000);
  virtual ~SplitStrobiePattern();

  // init the pattern to initial state
  virtual void init() override;
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

#ifdef TEST_FRAMEWORK
  virtual void saveTemplate(int level = 0) const override;
#endif

private:
  // the duration for the step
  uint16_t m_stepDuration;
  // the step timer
  Timer m_stepTimer;

  bool m_switch;
};

#endif 
