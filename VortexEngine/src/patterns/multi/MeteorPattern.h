#ifndef METEOR_PATTERN_H
#define METEOR_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../LedStash.h"

class MeteorPattern : public BlinkStepPattern
{
public:
  MeteorPattern();
  virtual ~MeteorPattern();

protected:
  virtual void blinkOn() override;
  virtual void blinkOff() override;
  virtual void poststep() override;

private:
  LedStash m_stash;
};
#endif
