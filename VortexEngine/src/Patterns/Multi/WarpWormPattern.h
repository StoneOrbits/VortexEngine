#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

class WarpWormPattern : public BlinkStepPattern
{
public:
  WarpWormPattern(uint8_t onDuration = DOPISH_ON_DURATION, uint8_t offDuration = DOPISH_OFF_DURATION, uint8_t stepDuration = 20);
  WarpWormPattern(const PatternArgs &args);
  virtual ~WarpWormPattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // how much the warp has progressed
  uint8_t m_progress;
};

