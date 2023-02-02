#ifndef FLOWERS_PATTERN_H
#define FLOWERS_PATTERN_H

#include "HybridPattern.h"

// Rabbit is PATTERN_STROBIE on tops and PATTERN_STROBE on tips
class FlowersPattern : public HybridPattern
{
public:
  FlowersPattern(uint8_t onDuration1 = 5, uint8_t offDuration1 = 8, uint8_t gapDuration1 = 0,
    uint8_t onDuration2 = 1, uint8_t offDuration2 = 3, uint8_t gapDuration2 = 0);
  FlowersPattern(const PatternArgs &args);
  virtual ~FlowersPattern();

  // init the pattern to initial state
  virtual void init() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

private:
  PatternArgs m_firstPatternArgs;
  PatternArgs m_secondPatternArgs;
};

#endif
