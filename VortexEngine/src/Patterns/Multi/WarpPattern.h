#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

class WarpPattern : public BlinkStepPattern
{
public:
  WarpPattern(uint8_t onDuration = 2, uint8_t offDuration = 8, uint8_t stepDuration = 100);
  WarpPattern(const PatternArgs &args);
  virtual ~WarpPattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // how much the warp has progressed
  uint8_t m_progress;
};

