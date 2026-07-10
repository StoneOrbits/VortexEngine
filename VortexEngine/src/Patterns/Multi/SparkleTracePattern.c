#include "SparkleTracePattern.h"

static const PatternVTable SparkleTracePattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void SparkleTracePattern_init(SparkleTracePattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &SparkleTracePattern_vtable;
  self->base.base.base.patternID = PATTERN_SPARKLETRACE;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
