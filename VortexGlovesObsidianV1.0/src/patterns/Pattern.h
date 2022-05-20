#ifndef PATTERN_H
#define PATTERN_H

#include "../LedConfig.h"
#include "../Patterns.h"
#include "../Timer.h"

// The heirarchy of pattern currently looks like this:
//
//                                pattern*
//                              /        \
//                single led pat*           multi led pat*
//                /           \             /            \
//           basic pat       tracer pat    hybrid pat*    sequenced pat*
//           /                              /
//        gap pattern                 Rabbit Pattern
//        /
//     advanced pattern
//
// * = abstract class that cannot be instantiated

#define PATTERN_FLAGS_NONE  0

// the pattern is a multi-pattern
#define PATTERN_FLAG_MULTI  (1<<0)

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

  // must override the serialize routine to save the pattern
  virtual void serialize() const;
  // must override unserialize to load patterns
  virtual void unserialize();

  // Get the current relative tick number of the pattern
  uint32_t getPatternTick() const;

  // get/set the ID of the pattern (set by mode builder)
  PatternID getPatternID() const { return m_patternID; }

  // get the pattern flags
  uint32_t getFlags() const { return m_patternFlags; }

protected:
  // the ID of this pattern (set by pattern builder)
  PatternID m_patternID;
  // the collorset that is bound to this pattern
  Colorset *m_pColorset;
  // the Led the pattern is running on
  LedPos m_ledPos;

  // the tick number the pattern started playing on
  uint64_t m_patternStartTick;

  // any flags the pattern has
  uint32_t m_patternFlags;
};

#endif
