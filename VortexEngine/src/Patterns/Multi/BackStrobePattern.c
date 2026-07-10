#include "BackStrobePattern.h"

static const PatternVTable BackStrobePattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void BackStrobePattern_init(BackStrobePattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  CompoundPattern_init((CompoundPattern *)self, args);
  self->base.base.base.vtable = &BackStrobePattern_vtable;
  self->base.base.base.patternID = PATTERN_BACKSTROBE;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
