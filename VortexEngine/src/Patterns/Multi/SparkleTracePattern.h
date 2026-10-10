#ifndef SPARKLETRACE_PATTERN_H
#define SPARKLETRACE_PATTERN_H

#include <stdbool.h>
#include "BlinkStepPattern.h"
#include "../../Random/Random.h"

typedef struct SparkleTracePattern {
  // must remain the first member
  BlinkStepPattern base;
  Random m_randCtx;
} SparkleTracePattern;

// the blink/step vtable for this pattern
extern const BlinkStepVtable SparkleTracePattern_bvtable;

// initialize a new SparkleTracePattern
void SparkleTracePattern_init(SparkleTracePattern *self, const PatternArgs *args);
void SparkleTracePattern_destroy(Pattern *self);

void SparkleTracePattern_blinkOn(BlinkStepPattern *self);
void SparkleTracePattern_poststep(BlinkStepPattern *self);

#endif
