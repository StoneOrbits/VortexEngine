#ifndef PATTERN_H
#define PATTERN_H

#include "../Leds/LedTypes.h"
#include "../Patterns/Patterns.h"
#include "../Colors/Colorset.h"

// The heirarchy of pattern currently looks like this:
/*
 *                                pattern*
 *                              /        \
 *                single led pat*           multi led pat*
 *                /           \                / 
 *           basic pat       tracer pat       hybrid pat*
 *           /                               /           \
 *      Advanced pattern                 Rabbit Pattern   sequenced pattern
 *
 *     * = abstract class that cannot be instantiated
 */

#define PATTERN_FLAGS_NONE  0

// the pattern is a multi-pattern
#define PATTERN_FLAG_MULTI  (1<<0)

class ByteStream;

class PatternArgs
{
public:
  PatternArgs(uint8_t a1 = 0, uint8_t a2 = 0, uint8_t a3 = 0, uint8_t a4 = 0,
    uint8_t a5 = 0, uint8_t a6 = 0, uint8_t a7 = 0, uint8_t a8 = 0) :
    arg1(a1), arg2(a2), arg3(a3), arg4(a4), arg5(a5), arg6(a6), arg7(a7), arg8(a8)
  {
  }
  uint8_t arg1;
  uint8_t arg2;
  uint8_t arg3;
  uint8_t arg4;
  uint8_t arg5;
  uint8_t arg6;
  uint8_t arg7;
  uint8_t arg8;
};

class Pattern
{
  // PatternBuilder can access the Pattern internals
  friend class PatternBuilder;

protected:
  // Pattern is an abstract class
  Pattern();

  Pattern(const PatternArgs &args);

public:
  virtual ~Pattern();

  // bind a colorset and position to the pattern
  virtual void bind(const Colorset *colorset, LedPos pos);
  virtual void bind(LedPos pos);

  // init the pattern to initial state
  virtual void init();

  // pure virtual must override the play function
  virtual void play() = 0;

  // must override the serialize routine to save the pattern
  virtual void serialize(ByteStream &buffer) const;
  // must override unserialize to load patterns
  virtual void unserialize(ByteStream &buffer);

#if SAVE_TEMPLATE == 1
  // save the data template
  virtual void saveTemplate(int level = 0) const;
#endif

  // comparison to other pattern
  // NOTE: That may cause problems because the parameter is still a Pattern *
  //       which means comparison would need to cast the other upwards first
  // NOTE2: Removing virtual because this probably shouldn't be overridden
  bool equals(const Pattern *other);

  // change the colorset
  virtual void setColorset(const Colorset *set);
  virtual void clearColorset();

  // change the led position
  void setLedPos(LedPos pos) { m_ledPos = pos; }

  // get/set the ID of the pattern (set by mode builder)
  PatternID getPatternID() const { return m_patternID; }

  // get a pointer to the colorset that is bound to the pattern
  const Colorset *getColorset() const { return &m_colorset; }
  Colorset *getColorset() { return &m_colorset; }
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
