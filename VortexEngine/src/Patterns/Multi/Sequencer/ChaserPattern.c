#include "../../Patterns/Multi/Sequencer/ChaserPattern.h"

static const PatternVTable ChaserPattern_vtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void ChaserPattern_init(ChaserPattern *self, const PatternArgs *args) {
#if VORTEX_SLIM == 0
  SequencedPattern_init((SequencedPattern *)self, args);
  self->base.base.base.base.vtable = &ChaserPattern_vtable;
  self->base.base.base.base.patternID = PATTERN_CHASER;
  Pattern_setArgs((Pattern *)self, args);
#else
  (void)self;
  (void)args;
#endif
}
