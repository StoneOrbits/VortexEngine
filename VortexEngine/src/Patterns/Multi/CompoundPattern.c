#include "CompoundPattern.h"

static const PatternVTable CompoundPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void CompoundPattern_init(CompoundPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  MultiLedPattern_init((MultiLedPattern *)self, args);
  self->base.base.vtable = &CompoundPattern_vtable;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
