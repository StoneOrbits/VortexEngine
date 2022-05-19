#ifndef PATTERN_BUILDER_H
#define PATTERN_BUILDER_H

#include "Patterns.h"

class Pattern;
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

private:
  // helper routine
  static Pattern *generate(PatternID id);
};

#endif
