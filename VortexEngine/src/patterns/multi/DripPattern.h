#ifndef DRIP_PATTERN_H
#define DRIP_PATTERN_H

#include "BlinkStepPattern.h"

class DripPattern : public BlinkStepPattern
{
public:
  DripPattern();
  virtual ~DripPattern();

  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  bool m_sync;
};

#endif
