#ifndef MULTI_LED_PATTERN_H
#define MULTI_LED_PATTERN_H

#include "Pattern.h"

class MultiLedPattern : public Pattern
{
public:
  MultiLedPattern();
  virtual ~MultiLedPattern();

  virtual void bind(const Colorset *set);

  // init the pattern to initial state
  virtual void init();

private:

};

#endif
