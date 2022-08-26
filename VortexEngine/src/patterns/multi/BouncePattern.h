#include "BlinkStepPattern.h"

class BouncePattern : public BlinkStepPattern
{
public:
  BouncePattern(uint8_t stepDuration = 50, uint8_t snakeSize = 1, uint8_t fadeAmount = 55);
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

