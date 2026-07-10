#include "DripPattern.h"

static const PatternVTable DripPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void DripPattern_init(DripPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &DripPattern_vtable;
  self->base.base.base.patternID = PATTERN_DRIP;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
