#ifndef RABBIT_PATTERN_H
#define RABBIT_PATTERN_H

#include "HybridPattern.h"

class RabbitPattern : public HybridPattern
{
public:
  RabbitPattern();
  virtual ~RabbitPattern();

  // init the pattern to initial state
  virtual void init() override;

private:
};

#endif
