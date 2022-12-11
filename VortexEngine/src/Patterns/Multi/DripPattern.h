#ifndef DRIP_PATTERN_H
#define DRIP_PATTERN_H

#include "BlinkStepPattern.h"

class DripPattern : public BlinkStepPattern
{
public:
  DripPattern(uint8_t onDuration = 10 , uint8_t offDuration = 10, uint8_t stepDuration = 100);
; DripPattern(const PatternArgs &args);
  virtual ~DripPattern();

  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  bool m_sync;
};

#endif
