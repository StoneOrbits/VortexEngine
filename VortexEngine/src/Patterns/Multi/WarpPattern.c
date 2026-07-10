#include "WarpPattern.h"

static const PatternVTable WarpPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void WarpPattern_init(WarpPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &WarpPattern_vtable;
  self->base.base.base.patternID = PATTERN_WARP;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
