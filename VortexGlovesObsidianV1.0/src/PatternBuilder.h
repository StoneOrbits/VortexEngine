#ifndef PATTERN_BUILDER_H
#define PATTERN_BUILDER_H

#include "Patterns.h"

class Pattern;

class PatternBuilder
{
public:
  static Pattern *make(PatternID id);

private:
  // helper routine
  static Pattern *generate(PatternID id);
};

#endif
