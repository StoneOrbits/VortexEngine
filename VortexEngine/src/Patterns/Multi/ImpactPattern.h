#ifndef IMPACT_PATTERN_H
#define IMPACT_PATTERN_H

#include "HybridPattern.h"

class ImpactPattern : public HybridPattern
{
public:
  ImpactPattern();
  ImpactPattern(const PatternArgs &args);
  virtual ~ImpactPattern();

  // init the pattern to initial state
  virtual void init() override;

private:
};

#endif 
