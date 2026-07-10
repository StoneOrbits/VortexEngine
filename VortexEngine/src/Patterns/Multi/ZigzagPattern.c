#include "ZigzagPattern.h"

static const PatternVTable ZigzagPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void ZigzagPattern_init(ZigzagPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  MultiLedPattern_init((MultiLedPattern *)self, args);
  self->base.base.vtable = &ZigzagPattern_vtable;
  self->base.base.patternID = PATTERN_ZIGZAG;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
