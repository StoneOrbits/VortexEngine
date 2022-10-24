#include "BlinkStepPattern.h"

#include "../../Timings.h"

class BouncePattern : public BlinkStepPattern
{
public:
  BouncePattern(int8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION, uint8_t stepDuration = 50);
  virtual ~BouncePattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // how much the warp has progressed
  uint8_t m_progress;
};

