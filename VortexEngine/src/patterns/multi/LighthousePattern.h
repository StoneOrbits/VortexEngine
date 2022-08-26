#include "BlinkStepPattern.h"

#include "../../Timer.h"
#include "../../LedStash.h"

class LighthousePattern : public BlinkStepPattern
{
public:
  LighthousePattern(uint8_t stepDuration = 50, uint8_t snakeSize = 1, uint8_t fadeAmount = 55);
  virtual ~LighthousePattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

protected:
  // overrideable members:
  virtual void blinkOn() override;
  virtual void blinkOff() override;
  virtual void poststep() override;

  // new function for this pattern
  virtual void fade();

private:
  // a timer for controling fade frqeuency
  Timer m_fadeTimer;
  // storage for leds
  LedStash m_stash;
  // how much the warp has progressed
  uint8_t m_progress;
};