#ifndef PATTERN_H
#define PATTERN_H

#include "../LedConfig.h"

class Colorset;
class LedControl;
class TimeControl;

class Pattern
{
  public:
    Pattern();
    virtual ~Pattern();

    // pure virtual must override the play function
    virtual void play(Colorset *colorset, LedPos pos) = 0;

    // must override the serialize routine to save the pattern
    virtual void serialize() const = 0;
    // must override unserialize to load patterns
    virtual void unserialize() = 0;

  private:
};

#endif
