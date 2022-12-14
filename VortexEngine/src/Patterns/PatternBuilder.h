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

  // generate a single LED pattern (nullptr if patternid is not single LED)
  static SingleLedPattern *makeSingle(PatternID id, const PatternArgs *args = nullptr);

  // generate a multi LED pattern (nullptr if patternid is not multi LED)
  static MultiLedPattern *makeMulti(PatternID id, const PatternArgs *args = nullptr);

  // unserialize a buffer into a pattern
  static Pattern *unserialize(ByteStream &buffer);

  // get default args for any given pattern
  static PatternArgs getDefaultArgs(PatternID id);

private:
  // helper routines
  static Pattern *makeInternal(PatternID id, const PatternArgs *args = nullptr);
  static Pattern *generate(PatternID id, const PatternArgs *args = nullptr);
};

#endif
