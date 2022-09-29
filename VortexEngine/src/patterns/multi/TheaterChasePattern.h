#ifndef THEATER_CHASE_PATTERN_H
#define THEATER_CHASE_PATTERN_H

#include "BlinkStepPattern.h"

// Rabbit is PATTERN_STROBIE on tops and PATTERN_STROBE on tips
class TheaterChasePattern : public BlinkStepPattern
{
public:
  TheaterChasePattern();
  virtual ~TheaterChasePattern();

  virtual void init();

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

  LedMap m_ledPositions;
  uint8_t m_stepCounter;
};

#endif
