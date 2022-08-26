#ifndef TIPTOP_PATTERN_H
#define TIPTOP_PATTERN_H

#include "HybridPattern.h"

class TipTopPattern : public HybridPattern
{
public:
  TipTopPattern();
  virtual ~TipTopPattern();

  // init the pattern to initial state
  virtual void init() override;

private:
};
#endif
