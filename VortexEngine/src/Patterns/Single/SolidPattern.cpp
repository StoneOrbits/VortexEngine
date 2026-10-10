#include "SolidPattern.h"

#include "../../Colors/Colorset.h"
#include "../../Memory/Memory.h"
#include "../PatternBuilder.h"

// the solid vtable, only onBlinkOn differs from the blink vtable
const BlinkPatternVtable SolidPattern_bvtable = {
  .base = {
    .destroy = BlinkPattern_destroy,
    .bind = BlinkPattern_bind,
    .init = BlinkPattern_doInit,
    .play = BlinkPattern_play
  },
  .onBlinkOn = SolidPattern_onBlinkOn,
  .onBlinkOff = BlinkPattern_onBlinkOff,
  .beginGap = BlinkPattern_beginGap,
  .beginDash = BlinkPattern_beginDash
};

// the only hook SolidPattern changes, it positions the colorset before
// handing off to the regular blink behaviour
void SolidPattern_onBlinkOn(BlinkPattern *bpat)
{
  SolidPattern *self = (SolidPattern *)bpat;
  if (self->m_colIndex == 0) {
    Colorset_resetIndex(&bpat->base.m_colorset);
  } else {
    Colorset_setCurIndex(&bpat->base.m_colorset, self->m_colIndex - 1);
  }
  // the original called the *base* (non blend) implementation here, and
  // because m_isBlend is false for PATTERN_SOLID the dispatch lands there
  BlinkPattern_onBlinkOn(bpat);
}

void SolidPattern_init(SolidPattern *self, PatternID id, const PatternArgs *args)
{
  // this registers the five blink arguments, matching the order the old
  // SolidPattern constructor produced by calling the base constructor first
  BlinkPattern_initWithVtable(&self->base, id, args, &SolidPattern_bvtable);

  self->m_colIndex = 0;
  // the colIndex argument stays registered as the sixth and last one so
  // the serialized form and the editor layout are unchanged
  REGISTER_ARG((Pattern *)&self->base.base, SolidPattern, m_colIndex);

  // re-apply the arguments now that the extra one is registered
  Pattern_setArgs(&self->base.base, args);
}

SolidPattern *SolidPattern_make(PatternID id, const PatternArgs *args)
{
  SolidPattern *self = (SolidPattern *)PatternBuilder_allocPattern();
  if (!self) {
    return NULL;
  }
  SolidPattern_init(self, id, args);
  return self;
}
