#ifndef IMPACT_PATTERN_H
#define IMPACT_PATTERN_H

#include "HybridPattern.h"

// Rabbit is PATTERN_STROBIE on tops and PATTERN_STROBE on tips
class ImpactPattern : public HybridPattern
{
public:
  ImpactPattern();
  virtual ~ImpactPattern();

  // init the pattern to initial state
  virtual void init() override;

private:
};

#endif 
