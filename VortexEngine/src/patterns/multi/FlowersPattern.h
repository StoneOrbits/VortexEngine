#ifndef FLOWERS_PATTERN_H
#define FLOWERS_PATTERN_H

#include "HybridPattern.h"

// Rabbit is PATTERN_STROBIE on tops and PATTERN_STROBE on tips
class FlowersPattern : public HybridPattern
{
public:
  FlowersPattern();
  virtual ~FlowersPattern();

  // init the pattern to initial state
  virtual void init() override;

private:
};

#endif 