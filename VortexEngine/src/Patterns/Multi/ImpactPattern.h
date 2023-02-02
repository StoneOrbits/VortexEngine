#ifndef IMPACT_PATTERN_H
#define IMPACT_PATTERN_H

#include "HybridPattern.h"

#include "../../Time/Timings.h"

class ImpactPattern : public HybridPattern
{
public:
  ImpactPattern(uint8_t onDuration1 = 2, uint8_t offDuration1 = 8, uint8_t onDuration2 = STROBE_ON_DURATION,
    uint8_t offDuration2 = STROBE_OFF_DURATION, uint8_t onDuration3 = 25, uint8_t offDuration3 = 20);
  ImpactPattern(const PatternArgs &args);
  virtual ~ImpactPattern();

  // init the pattern to initial state
  virtual void init() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

private:
  PatternArgs m_firstSideArgs;
  PatternArgs m_secondSideArgs;
  PatternArgs m_otherArgs;
};

#endif
