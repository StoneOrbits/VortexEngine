#include "TheaterChasePattern.h"

static const PatternVTable TheaterChasePattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void TheaterChasePattern_init(TheaterChasePattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  BlinkStepPattern_init((BlinkStepPattern *)self, args);
  self->base.base.base.vtable = &TheaterChasePattern_vtable;
  self->base.base.base.patternID = PATTERN_THEATER_CHASE;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
