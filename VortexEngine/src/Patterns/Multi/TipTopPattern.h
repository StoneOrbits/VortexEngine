#ifndef TIPTOP_PATTERN_H
#define TIPTOP_PATTERN_H

#include "HybridPattern.h"

class TipTopPattern : public HybridPattern
{
public:
  TipTopPattern(uint8_t onDuration1 = 32, uint8_t offDuration1 = 28, uint8_t gapDuration1 = 0,
    uint8_t onDuration2 = 8, uint8_t offDuration2 = 10, uint8_t gapDuration2 = 0);
  TipTopPattern(const PatternArgs &args);
  virtual ~TipTopPattern();

  // init the pattern to initial state
  virtual void init() override;

  virtual void setArgs(const PatternArgs& args) override;
  virtual void getArgs(PatternArgs& args) const override;

private:

  PatternArgs m_tipArgs;
  PatternArgs m_topArgs;
};
#endif
