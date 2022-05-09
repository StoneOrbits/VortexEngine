#ifndef SINGLE_LED_PATTERN_H
#define SINGLE_LED_PATTERN_H

#include "Pattern.h"

class SingleLedPattern : public Pattern
{
public:
  SingleLedPattern();
  virtual ~SingleLedPattern();

  virtual void init(Colorset *colorset, LedPos pos);

  // skip the pattern ahead some ticks
  virtual void skip(uint32_t ticks);

  // resume a pattern from the current time, use this is the pattern
  // stops for some duration of time and then needs to play again
  // without having changed state at all. Otherwise the change in time
  // will cause the timer to behave differently
  virtual void resume();

private:

};

#endif
