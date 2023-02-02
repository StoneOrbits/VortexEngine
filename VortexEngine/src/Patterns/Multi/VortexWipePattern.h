#ifndef VORTEXWIPE_PATTERN_H
#define VORTEXWIPE_PATTERN_h

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

class VortexWipePattern : public BlinkStepPattern
{
public:
  VortexWipePattern(uint8_t onDuration = 2, uint8_t offDuration = 4, uint8_t stepDuration = 75);
  VortexWipePattern(const PatternArgs &args);
  virtual ~VortexWipePattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // path for leds to take, index this with m_step up to LED_COUNT steps
  static const LedPair ledStepPairs[];

  // how much the fill has progressed
  uint8_t m_progress;
};

#endif
