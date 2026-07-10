#include "CrossDopsPattern.h"

static const PatternVTable CrossDopsPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void CrossDopsPattern_init(CrossDopsPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &CrossDopsPattern_vtable;
  self->base.base.base.patternID = PATTERN_CROSSDOPS;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
