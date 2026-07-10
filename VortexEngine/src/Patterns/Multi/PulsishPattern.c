#include "PulsishPattern.h"

static const PatternVTable PulsishPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void PulsishPattern_init(PulsishPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  MultiLedPattern_init((MultiLedPattern *)self, args);
  self->base.base.vtable = &PulsishPattern_vtable;
  self->base.base.patternID = PATTERN_PULSISH;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
