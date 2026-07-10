#include "SnowballPattern.h"

static const PatternVTable SnowballPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void SnowballPattern_init(SnowballPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &SnowballPattern_vtable;
  self->base.base.base.patternID = PATTERN_SNOWBALL;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
