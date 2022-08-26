#ifndef CROSSDOPS_PATTERN_H
#define CROSSDOPS_PATTERN_H

#include "BlinkStepPattern.h"

class CrossDopsPattern : public BlinkStepPattern
{
public:
  CrossDopsPattern();
  virtual ~CrossDopsPattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  // overrideable members:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  LedMap m_ledMap;
};

#endif
