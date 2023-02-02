#ifndef CROSSDOPS_PATTERN_H
#define CROSSDOPS_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

class CrossDopsPattern : public BlinkStepPattern
{
public:
  CrossDopsPattern(uint8_t onDuration = 2, uint8_t offDuration = 12, uint8_t stepDuration = 50);
  CrossDopsPattern(const PatternArgs &args);
  virtual ~CrossDopsPattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  // overrideable members:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  bool m_switch;
};

#endif
