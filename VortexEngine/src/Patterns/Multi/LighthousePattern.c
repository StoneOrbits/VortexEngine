#include "LighthousePattern.h"

static const PatternVTable LighthousePattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void LighthousePattern_init(LighthousePattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &LighthousePattern_vtable;
  self->base.base.base.patternID = PATTERN_LIGHTHOUSE;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
