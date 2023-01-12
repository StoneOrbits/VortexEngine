#ifndef IMPACT_PATTERN_H
#define IMPACT_PATTERN_H

#include "HybridPattern.h"

class ImpactPattern : public HybridPattern
{
public:
  ImpactPattern(uint8_t onDuration1 = 32, uint8_t offDuration1 = 250, uint8_t onDuration2 = 8,
    uint8_t offDuration2 = 10, uint8_t onDuration3 = 3, uint8_t offDuration3 = 28);
  ImpactPattern(const PatternArgs &args);
  virtual ~ImpactPattern();

  // init the pattern to initial state
  virtual void init() override;
 
  // must override the serialize routine to save the pattern
  virtual void serialize(ByteStream& buffer) const override;
  virtual void unserialize(ByteStream& buffer) override;

  virtual void setArgs(const PatternArgs& args) override;
  virtual void getArgs(PatternArgs& args) const override;

private:
  PatternArgs m_thumbArgs;
  PatternArgs m_middleArgs;
  PatternArgs m_otherArgs;
};

#endif 
