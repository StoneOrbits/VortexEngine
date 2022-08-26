#ifndef DOUBLESTROBE_PATTERN_H
#define DOUBLESTROBE_PATTERN_H

#include "BlinkStepPattern.h"

class DoubleStrobePattern : public BlinkStepPattern
{
public:
  DoubleStrobePattern();
  virtual ~DoubleStrobePattern();

  virtual void init() override;

protected:
  virtual void blinkOn() override;
};
#endif
