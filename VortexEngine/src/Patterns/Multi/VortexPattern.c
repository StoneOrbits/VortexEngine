#include "VortexPattern.h"

static const PatternVTable VortexPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void VortexPattern_init(VortexPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &VortexPattern_vtable;
  self->base.base.base.patternID = PATTERN_VORTEX;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
