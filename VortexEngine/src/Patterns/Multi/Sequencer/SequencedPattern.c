#include "SequencedPattern.h"

static const PatternVTable SequencedPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void SequencedPattern_init(SequencedPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  CompoundPattern_init((CompoundPattern *)self, args);
  self->base.base.base.vtable = &SequencedPattern_vtable;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
