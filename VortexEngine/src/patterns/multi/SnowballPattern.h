#include "BlinkStepPattern.h"

#include "../../Timer.h"

class SnowballPattern : public BlinkStepPattern
{
public:
  SnowballPattern(uint8_t stepDuration = 50, uint8_t snakeSize = 1, uint8_t fadeAmount = 55);
  virtual ~SnowballPattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // how much the warp has progressed
  uint8_t m_progress;
};
