#ifndef SINGLE_LED_PATTERN_H
#define SINGLE_LED_PATTERN_H

#include "../Pattern.h"

class SingleLedPattern : public Pattern
{
protected:
  // SingleLedPattern is an abstract class
  SingleLedPattern(const PatternArgs &args);

public:
  virtual ~SingleLedPattern();

  virtual void init() override;

  // skip the pattern ahead some ticks
  // NOTE: This is not working properly at the moment
  virtual void skip(uint32_t ticks);

private:

};

#endif
