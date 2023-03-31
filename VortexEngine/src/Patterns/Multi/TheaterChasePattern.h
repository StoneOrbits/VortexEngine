#ifndef THEATER_CHASE_PATTERN_H
#define THEATER_CHASE_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

class TheaterChasePattern : public BlinkStepPattern
{
public:
  TheaterChasePattern(uint8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION, uint8_t stepDuration = 28);
  TheaterChasePattern(const PatternArgs &args);
  virtual ~TheaterChasePattern();

  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

  LedMap m_ledPositions;
  uint8_t m_stepCounter;
};

#endif
