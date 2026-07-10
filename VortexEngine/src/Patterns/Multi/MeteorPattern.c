#include "MeteorPattern.h"

static const PatternVTable MeteorPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void MeteorPattern_init(MeteorPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &MeteorPattern_vtable;
  self->base.base.base.patternID = PATTERN_METEOR;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
