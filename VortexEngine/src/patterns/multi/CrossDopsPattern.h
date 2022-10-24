#ifndef CROSSDOPS_PATTERN_H
#define CROSSDOPS_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Timings.h"

class CrossDopsPattern : public BlinkStepPattern
{
public:
  CrossDopsPattern(uint8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION, uint8_t stepDuration = 50);
  virtual ~CrossDopsPattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  // overrideable members:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  LedMap m_ledMap;
};

#endif
