#include "SolidPattern.h"

const PatternVTable SolidPattern_vtable = {
  NULL,
  BasicPattern_play,
  BasicPattern_initVirtual,
  Pattern_bindBase,
  SolidPattern_onBlinkOn,
  BasicPattern_onBlinkOff,
  BasicPattern_beginGap,
  BasicPattern_beginDash
};

void SolidPattern_init(SolidPattern *self, const PatternArgs *args)
{
  BasicPattern_init(&self->base, args);
  self->base.base.base.vtable = &SolidPattern_vtable;
  self->colIndex = 0;
  self->base.base.base.patternID = PATTERN_SOLID;
  REGISTER_ARG(self, self->colIndex);
  Pattern_setArgs(&self->base.base.base, args);
}

void SolidPattern_onBlinkOn(Pattern *base)
{
  SolidPattern *self = (SolidPattern *)base;
  if (self->colIndex == 0) {
    Colorset_resetIndex(&base->colorset);
  } else {
    Colorset_setCurIndex(&base->colorset, self->colIndex - 1);
  }
  BasicPattern_onBlinkOn(base);
}
