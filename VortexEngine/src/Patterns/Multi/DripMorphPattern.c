#include "DripMorphPattern.h"

static const PatternVTable DripMorphPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void DripMorphPattern_init(DripMorphPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  MultiLedPattern_init((MultiLedPattern *)self, args);
  self->base.base.vtable = &DripMorphPattern_vtable;
  self->base.base.patternID = PATTERN_DRIPMORPH;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
