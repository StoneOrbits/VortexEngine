#ifndef DOUBLESTROBE_PATTERN_H
#define DOUBLESTROBE_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

class DoubleStrobePattern : public BlinkStepPattern
{
public:
  DoubleStrobePattern(uint8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION, uint8_t stepDuration = 100);
  DoubleStrobePattern(const PatternArgs &args);
  virtual ~DoubleStrobePattern();

  virtual void init() override;

protected:
  virtual void blinkOn() override;
};
#endif
