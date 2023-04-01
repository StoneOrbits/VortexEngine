#ifndef METEOR_PATTERN_H
#define METEOR_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"
#include "../../Leds/LedStash.h"

class MeteorPattern : public BlinkStepPattern
{
public:
  MeteorPattern(const PatternArgs &args);
  virtual ~MeteorPattern();

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
