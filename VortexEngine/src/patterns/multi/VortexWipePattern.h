#ifndef VORTEXWIPE_PATTERN_H
#define VORTEXWIPE_PATTERN_h

#include "BlinkStepPattern.h"

class VortexWipePattern : public BlinkStepPattern
{
public:
  VortexWipePattern(uint8_t stepDuration = 50, uint8_t snakeSize = 1, uint8_t fadeAmount = 55);
  virtual ~VortexWipePattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // path for leds to take, index this with m_step up to LED_COUNT steps
  static const LedPos ledStepPositions[];

  // how much the fill has progressed
  uint8_t m_progress;
};

#endif
