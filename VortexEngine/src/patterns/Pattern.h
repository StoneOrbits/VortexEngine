#ifndef PATTERN_H
#define PATTERN_H

#include "../LedConfig.h"
#include "../Patterns.h"
#include "../Colorset.h"

// The heirarchy of pattern currently looks like this:
/*
 *                                pattern*
 *                              /        \
 *                single led pat*           multi led pat*
 *                /           \             /            \
 *           basic pat       tracer pat    hybrid pat*    sequenced pat*
 *           /                              /
 *      Advanced pattern                 Rabbit Pattern
 *
 *     * = abstract class that cannot be instantiated
 */

#define PATTERN_FLAGS_NONE  0

// the pattern is a multi-pattern
#define PATTERN_FLAG_MULTI  (1<<0)

class SerialBuffer;

class Pattern
{
  // PatternBuilder can access the Pattern internals
  friend class PatternBuilder;

protected:
  // Pattern is an abstract class
  Pattern();

public:
  virtual ~Pattern();

  // bind a colorset and position to the pattern and initialize
  virtual void bind(const Colorset *colorset, LedPos pos);

  // init the pattern to initial state
  virtual void init();

  // pure virtual must override the play function
  virtual void play() = 0;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer &buffer) const;
  // must override unserialize to load patterns
  virtual void unserialize(SerialBuffer &buffer);

  // comparison to other pattern
  virtual bool equals(const Pattern *other);

  // change the colorset
  virtual void setColorset(const Colorset *set);
  virtual void clearColorset();

  // change the led position
  void setLedPos(LedPos pos) { m_ledPos = pos; }

  // get/set the ID of the pattern (set by mode builder)
  PatternID getPatternID() const { return m_patternID; }

  // get a pointer to the colorset that is bound to the pattern
  const Colorset *getColorset() const { return &m_colorset; }
  LedPos getLedPos() const { return m_ledPos; }

  // get the pattern flags
  uint32_t getFlags() const { return m_patternFlags; }
  bool hasFlags(uint32_t flags) const { return (m_patternFlags & flags) != 0; }

protected:
  // the ID of this pattern (set by pattern builder)
  PatternID m_patternID;
  // any flags the pattern has
  uint8_t m_patternFlags;
  // a copy of the colorset that this pattern is initialized with
  Colorset m_colorset;
  // the Led the pattern is running on
  LedPos m_ledPos;
};

#endif
