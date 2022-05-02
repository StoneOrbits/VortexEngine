#ifndef PATTERN_H
#define PATTERN_H

#include "../LedConfig.h"
#include "../Patterns.h"
#include "../Timer.h"

class Colorset;

class Pattern
{
  friend class PatternBuilder;

public:
  Pattern();
  virtual ~Pattern();

  // init the pattern to initial state
  virtual void init(Colorset *colorset, LedPos pos);

  // pure virtual must override the play function
  virtual void play() = 0;

  // skip the pattern ahead some ticks
  virtual void skip(uint32_t ticks);

  // must override the serialize routine to save the pattern
  virtual void serialize() const;
  // must override unserialize to load patterns
  virtual void unserialize();

  // get/set the ID of the pattern (set by mode builder)
  PatternID getPatternID() const { return m_patternID; }

protected:
  // the ID of this pattern (set by pattern builder)
  PatternID m_patternID;
  // the collorset that is bound to this pattern
  Colorset *m_pColorset;
  // the Led the pattern is running on
  LedPos m_ledPos;
};

#endif
