#include "VortexWipePattern.h"

static const PatternVTable VortexWipePattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void VortexWipePattern_init(VortexWipePattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &VortexWipePattern_vtable;
  self->base.base.base.patternID = PATTERN_VORTEXWIPE;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
