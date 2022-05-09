#ifndef MULTI_LED_PATTERN_H
#define MULTI_LED_PATTERN_H

#include "Pattern.h"

class MultiLedPattern : public Pattern
{
public:
  MultiLedPattern();
  virtual ~MultiLedPattern();

  // init the pattern to initial state
  virtual void init(Colorset *colorset, LedPos pos);

  // pure virtual must override the play function
  virtual void play() = 0;

  // must override the serialize routine to save the pattern
  virtual void serialize() const;
  // must override unserialize to load patterns
  virtual void unserialize();

private:

};

#endif
