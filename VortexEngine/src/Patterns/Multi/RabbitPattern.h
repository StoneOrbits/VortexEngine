#ifndef RABBIT_PATTERN_H
#define RABBIT_PATTERN_H

#include "HybridPattern.h"

class RabbitPattern : public HybridPattern
{
public:
  RabbitPattern(uint8_t onDuration1 = 8, uint8_t offDuration1 = 10, uint8_t gapDuration1 = 0,
    uint8_t onDuration2 = 3, uint8_t offDuration2 = 28, uint8_t gapDuration2 = 0);
  RabbitPattern(const PatternArgs &args);
  virtual ~RabbitPattern();

  // init the pattern to initial state
  virtual void init() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(ByteStream& buffer) const override;
  virtual void unserialize(ByteStream& buffer) override;

  virtual void setArgs(const PatternArgs& args) override;
  virtual void getArgs(PatternArgs& args) const override;

private:
  PatternArgs m_tipArgs;
  PatternArgs m_topArgs;
};

#endif
