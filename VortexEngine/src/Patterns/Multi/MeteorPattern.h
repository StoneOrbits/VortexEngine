#ifndef METEOR_PATTERN_H
#define METEOR_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"
#include "../../Leds/LedStash.h"

class MeteorPattern : public BlinkStepPattern
{
public:
  MeteorPattern(uint8_t onDuration = STROBE_ON_DURATION, uint8_t offDuration = STROBE_OFF_DURATION, uint8_t stepDuration = 55, uint8_t fadeAmount = 75);
  MeteorPattern(const PatternArgs &args);
  virtual ~MeteorPattern();

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

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
