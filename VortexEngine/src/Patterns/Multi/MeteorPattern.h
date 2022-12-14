#ifndef METEOR_PATTERN_H
#define METEOR_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"
#include "../../Leds/LedStash.h"

class MeteorPattern : public BlinkStepPattern
{
public:
  MeteorPattern(uint8_t onDuration = STROBE_ON_DURATION, uint8_t offDuration = STROBE_OFF_DURATION, uint8_t stepDuration = 50, uint8_t fadeAmount = 15);
  MeteorPattern(const PatternArgs &args);
  virtual ~MeteorPattern();

  // must override the serialize routine to save the pattern
  virtual void serialize(ByteStream& buffer) const override;
  virtual void unserialize(ByteStream& buffer) override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level = 0) const override;
#endif

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
