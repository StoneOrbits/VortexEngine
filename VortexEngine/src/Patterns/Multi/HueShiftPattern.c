#include "HueShiftPattern.h"

static const PatternVTable HueShiftPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void HueShiftPattern_init(HueShiftPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  MultiLedPattern_init((MultiLedPattern *)self, args);
  self->base.base.vtable = &HueShiftPattern_vtable;
  self->base.base.patternID = PATTERN_HUE_SCROLL;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
