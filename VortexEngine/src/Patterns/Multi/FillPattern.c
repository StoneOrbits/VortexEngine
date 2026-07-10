#include "FillPattern.h"

static const PatternVTable FillPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void FillPattern_init(FillPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &FillPattern_vtable;
  self->base.base.base.patternID = PATTERN_FILL;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
