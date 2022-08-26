#ifndef JEST_PATTERN_H
#define JEST_PATTERN_H

#include "HybridPattern.h"

// Rabbit is PATTERN_STROBIE on tops and PATTERN_STROBE on tips
class JestPattern : public HybridPattern
{
public:
  JestPattern();
  virtual ~JestPattern();

  // init the pattern to initial state
  virtual void init() override;

private:
};

#endif 
