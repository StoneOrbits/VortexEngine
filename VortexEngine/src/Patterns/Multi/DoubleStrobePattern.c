#include "DoubleStrobePattern.h"

static const PatternVTable DoubleStrobePattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void DoubleStrobePattern_init(DoubleStrobePattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &DoubleStrobePattern_vtable;
  self->base.base.base.patternID = PATTERN_DOUBLESTROBE;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
