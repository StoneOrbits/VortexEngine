#ifndef THEATER_CHASE_PATTERN_H
#define THEATER_CHASE_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

// Rabbit is PATTERN_STROBIE on tops and PATTERN_STROBE on tips
class TheaterChasePattern : public BlinkStepPattern
{
public:
  TheaterChasePattern(uint8_t onDuration = 1, uint8_t offDuration = 3, uint8_t stepDuration = 28);
  TheaterChasePattern(const PatternArgs &args);
  virtual ~TheaterChasePattern();

  virtual void init();

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

  uint8_t m_stepCounter;
};

#endif
