#ifndef FILL_PATTERN_H
#define FILL_PATTERN_h

#include "BlinkStepPattern.h"

class FillPattern : public BlinkStepPattern
{
public:
  FillPattern();
  virtual ~FillPattern();

  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // how much the fill has progressed
  uint8_t m_progress;
};

#endif
