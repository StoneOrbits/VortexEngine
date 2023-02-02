#ifndef RABBIT_PATTERN_H
#define RABBIT_PATTERN_H

#include "HybridPattern.h"

class RabbitPattern : public HybridPattern
{
public:
  RabbitPattern(uint8_t onDuration1 = 1, uint8_t offDuration1 = 2, uint8_t gapDuration1 = 5,
    uint8_t onDuration2 = 3, uint8_t offDuration2 = 12, uint8_t gapDuration2 = 0);
  RabbitPattern(const PatternArgs &args);
  virtual ~RabbitPattern();

  // init the pattern to initial state
  virtual void init() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

private:
  PatternArgs m_tipArgs;
  PatternArgs m_topArgs;
};

#endif
