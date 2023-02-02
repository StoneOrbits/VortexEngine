#ifndef JEST_PATTERN_H
#define JEST_PATTERN_H

#include "HybridPattern.h"

// Rabbit is PATTERN_STROBIE on tops and PATTERN_STROBE on tips
class JestPattern : public HybridPattern
{
public:
  JestPattern(uint8_t onDuration = 1, uint8_t offDuration = 1, uint8_t gapDuration1 = 10,
    uint8_t gapDuration2 = 1, uint8_t groupSize = 3);
  JestPattern(const PatternArgs &args);
  virtual ~JestPattern();

  // init the pattern to initial state
  virtual void init() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

private:
  PatternArgs m_tipArgs;
  PatternArgs m_topArgs;
};

#endif
