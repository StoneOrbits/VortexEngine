#ifndef SINGLE_LED_PATTERN_H
#define SINGLE_LED_PATTERN_H

#include "../Pattern.h"

typedef struct SingleLedPattern {
  Pattern base;
} SingleLedPattern;

void SingleLedPattern_init(SingleLedPattern *self, const PatternArgs *args);

#endif
