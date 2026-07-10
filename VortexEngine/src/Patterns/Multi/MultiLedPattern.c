#include "MultiLedPattern.h"

#include "../Pattern.h"

const PatternVTable MultiLedPattern_vtable = {
  NULL,
  NULL,
  MultiLedPattern_initVirtual,
  MultiLedPattern_bind,
  NULL,
  NULL,
  NULL,
  NULL
};

void MultiLedPattern_init(MultiLedPattern *self, const PatternArgs *args)
{
  Pattern_init(&self->base, args);
  self->base.vtable = &MultiLedPattern_vtable;
  self->base.patternFlags |= PATTERN_FLAG_MULTI;
}

void MultiLedPattern_bind(Pattern *self, LedPos pos)
{
  (void)pos;
  Pattern_setLedPos(self, LED_COUNT);
}

void MultiLedPattern_initVirtual(Pattern *self)
{
  Pattern_initBase(self);
}
