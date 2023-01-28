#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

class BouncePattern : public BlinkStepPattern
{
public:
  BouncePattern(int8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION, uint8_t stepDuration = 200,
    uint8_t fadeAmount = 10);
  BouncePattern(const PatternArgs &args);
  virtual ~BouncePattern();

  // init the pattern to initial state
  virtual void init() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // how much the warp has progressed
  uint8_t m_progress;
  // the fade amount
  uint8_t m_fadeAmount;
};

