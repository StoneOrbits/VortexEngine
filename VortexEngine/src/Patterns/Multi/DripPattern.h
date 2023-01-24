#ifndef DRIP_PATTERN_H
#define DRIP_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"


class DripPattern : public BlinkStepPattern
{
public:
  DripPattern(uint8_t onDuration = STROBE_ON_DURATION, uint8_t offDuration = STROBE_OFF_DURATION, uint8_t stepDuration = 250);
  DripPattern(const PatternArgs &args);
  virtual ~DripPattern();

  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  bool m_sync;
};

#endif
