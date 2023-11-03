#ifndef PATTERN_BUILDER_H
#define PATTERN_BUILDER_H

#include "Patterns.h"

class Pattern;
class PatternArgs;
class ByteStream;
class MultiLedPattern;
class SingleLedPattern;

class PatternBuilder
{
public:
  // generic make any pattern
  static Pattern *make(PatternID id, const PatternArgs *args = nullptr);

  // make a copy of an existing pattern
  static Pattern *dupe(const Pattern *pat);

  // unserialize a buffer into a pattern
  static Pattern *unserialize(ByteStream &buffer);

  // This is just the default arguments for any given pattern id
  // it will *not* indicate the true amount of arguments a pattern has
  static PatternArgs getDefaultArgs(PatternID id);
  // this will give you actual amount of default args
  static uint8_t numDefaultArgs(PatternID id);

private:
  // helper routines
  static Pattern *makeInternal(PatternID id, const PatternArgs *args = nullptr);
  static Pattern *generate(PatternID id, const PatternArgs *args = nullptr);
};

#endif
