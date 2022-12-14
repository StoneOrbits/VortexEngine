#ifndef FILL_PATTERN_H
#define FILL_PATTERN_h

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

class FillPattern : public BlinkStepPattern
{
public:
  FillPattern(uint8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION, uint8_t stepDuration = 50);
  FillPattern(const PatternArgs &args);
  virtual ~FillPattern();

  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // how much the fill has progressed
  uint8_t m_progress;
};

#endif
