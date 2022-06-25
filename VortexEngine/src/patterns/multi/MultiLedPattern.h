#ifndef MULTI_LED_PATTERN_H
#define MULTI_LED_PATTERN_H

#include "../Pattern.h"

class MultiLedPattern : public Pattern
{
protected:
  // MultiLedPattern is an abstract class
  MultiLedPattern();

public:
  virtual ~MultiLedPattern();

  // multi-led and single-led have different bind functions
  virtual void bind(const Colorset *set);

  // init the pattern to initial state
  virtual void init() override;

private:

};

#endif
