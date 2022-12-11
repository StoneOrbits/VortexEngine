#ifndef SPARKLETRACE_PATTERN_H
#define SPARKLETRACE_PATTERN_H

#include "BlinkStepPattern.h"

class SparkleTracePattern : public BlinkStepPattern
{
public:
  SparkleTracePattern(uint8_t onDuration = 5, uint8_t offDuration = 0, uint8_t stepDuration = 50);
  SparkleTracePattern(const PatternArgs &args);
  virtual ~SparkleTracePattern();

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;
};
#endif