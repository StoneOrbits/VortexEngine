#include "MultiLedPattern.h"

const PatternVtable MultiLedPattern_vtable = {
  .destroy = Pattern_destroy,
  .bind = MultiLedPattern_bind,
  .init = MultiLedPattern_doInit,
  .play = Pattern_basePlay
};

void MultiLedPattern_init(MultiLedPattern *self, const PatternArgs *args, const PatternVtable *vtable)
{
  Pattern_initBase(&self->base, PATTERN_NONE, vtable ? vtable : &MultiLedPattern_vtable);
  // this is a multi led pattern
  Pattern_setFlags(&self->base, Pattern_getFlags(&self->base) | PATTERN_FLAG_MULTI);
  Pattern_setArgs(&self->base, args);
}

void MultiLedPattern_bind(Pattern *self, LedPos pos)
{
  // the 'LedPos' is LED_COUNT when binding a multi-led pattern
  // as opposed to being an individual LED when it's single
  Pattern_baseBind(self, LED_COUNT);
}

// init the pattern to initial state
void MultiLedPattern_doInit(Pattern *self)
{
  Pattern_baseInit(self);
}
