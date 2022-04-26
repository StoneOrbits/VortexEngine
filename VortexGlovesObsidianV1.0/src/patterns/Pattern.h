#ifndef PATTERN_H
#define PATTERN_H

#include "../LedConfig.h"
#include "../Patterns.h"

class Colorset;
class LedControl;
class TimeControl;

class Pattern
{
    friend class PatternBuilder;

  public:
    Pattern();
    virtual ~Pattern();

    // pure virtual must override the play function
    virtual void play(Colorset *colorset, LedPos pos) = 0;

    // must override the serialize routine to save the pattern
    virtual void serialize() const = 0;
    // must override unserialize to load patterns
    virtual void unserialize() = 0;

    // get/set the ID of the pattern (set by mode builder)
    PatternID getPatternID() const { return m_patternID; }

  private:
    // the ID of this pattern (set by pattern builder)
    PatternID m_patternID;
};

#endif
