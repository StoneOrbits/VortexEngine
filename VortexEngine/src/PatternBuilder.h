#ifndef PATTERN_BUILDER_H
#define PATTERN_BUILDER_H

#include "Patterns.h"

class Pattern;
class SerialBuffer;
class MultiLedPattern;
class SingleLedPattern;

class PatternBuilder
{
public:
  // generic make any pattern
  static Pattern *make(PatternID id);

  // generate a single LED pattern (nullptr if patternid is not single LED)
  static SingleLedPattern *makeSingle(PatternID id);

  // generate a multi LED pattern (nullptr if patternid is not multi LED)
  static MultiLedPattern *makeMulti(PatternID id);

  // unserialize a buffer into a pattern
  static Pattern *unserialize(SerialBuffer &buffer);

private:
  // helper routines
  static Pattern *createTheaterChase();
  static Pattern *createChaser();
  static Pattern *makeInternal(PatternID id);
  static Pattern *generate(PatternID id);
};

#endif
