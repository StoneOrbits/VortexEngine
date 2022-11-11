#include "BlinkStepPattern.h"

#include "../../LedStash.h"
#include "../../Timings.h"
#include "../../Timer.h"

class LighthousePattern : public BlinkStepPattern
{
public:
  LighthousePattern(uint8_t onDuration = DOPISH_ON_DURATION, uint8_t offDuration = DOPISH_OFF_DURATION, uint8_t stepDuration = 100,
    uint8_t fadeAmount = 25, uint8_t fadeRate = 5);
  virtual ~LighthousePattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

#ifdef TEST_FRAMEWORK
  virtual void saveTemplate(int level = 0) const override;
#endif

protected:
  // overrideable members:
  virtual void blinkOn() override;
  virtual void blinkOff() override;
  virtual void poststep() override;

  // new function for this pattern
  virtual void fade();

private:
  // the amount of fade to apply
  uint8_t m_fadeAmount;
  // the frequency at which the fade is applied
  uint8_t m_fadeRate;
  // a timer for controling fade frqeuency
  Timer m_fadeTimer;
  // storage for leds
  LedStash m_stash;
  // how much the warp has progressed
  uint8_t m_progress;
};
