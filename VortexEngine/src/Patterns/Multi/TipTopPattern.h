#ifndef TIPTOP_PATTERN_H
#define TIPTOP_PATTERN_H

#include "HybridPattern.h"

class TipTopPattern : public HybridPattern
{
public:
  TipTopPattern();
  TipTopPattern(const PatternArgs &args);
  virtual ~TipTopPattern();

  // init the pattern to initial state
  virtual void init() override;

private:
};
#endif
