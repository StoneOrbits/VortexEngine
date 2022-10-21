#ifndef METEOR_PATTERN_H
#define METEOR_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Timings.h"
#include "../../LedStash.h"

class MeteorPattern : public BlinkStepPattern
{
public:
  MeteorPattern(uint8_t onDuration = STROBE_ON_DURATION, uint8_t offDuration = STROBE_OFF_DURATION, uint8_t stepDuration = 50, uint8_t fadeAmount = 15);
  virtual ~MeteorPattern();

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

protected:
  virtual void blinkOn() override;
  virtual void blinkOff() override;
  virtual void poststep() override;

private:
  // the fade amount
  uint8_t m_fadeAmount;
  
  LedStash m_stash;
};
#endif
