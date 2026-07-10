#include "WarpWormPattern.h"

static const PatternVTable WarpWormPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void WarpWormPattern_init(WarpWormPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &WarpWormPattern_vtable;
  self->base.base.base.patternID = PATTERN_WARPWORM;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
