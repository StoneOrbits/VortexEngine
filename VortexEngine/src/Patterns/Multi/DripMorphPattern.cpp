#include "DripMorphPattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const PatternVtable DripMorphPattern_vtable = {
  .destroy = DripMorphPattern_destroy,
  .bind = MultiLedPattern_bind,
  .init = DripMorphPattern_initImpl,
  .play = DripMorphPattern_play
};

void DripMorphPattern_init(DripMorphPattern *self, const PatternArgs *args)
{
  MultiLedPattern_init(&self->base, args, &DripMorphPattern_vtable);
  self->m_blinkOnDuration = 0;
  self->m_blinkOffDuration = 0;
  Timer_make(&self->m_blinkTimer);
  self->m_speed = 0;
  self->m_cur = HSVColor_make();
  self->m_next = HSVColor_make();
  Pattern_setPatternID((&self->base.base), PATTERN_DRIPMORPH);
  REGISTER_ARG((&self->base.base), DripMorphPattern, m_blinkOnDuration);
  REGISTER_ARG((&self->base.base), DripMorphPattern, m_blinkOffDuration);
  REGISTER_ARG((&self->base.base), DripMorphPattern, m_speed);
  Pattern_setArgs((&self->base.base), args);
}

void DripMorphPattern_destroy(Pattern *pat)
{
  DripMorphPattern *self = (DripMorphPattern *)pat;
  (void)self;
}

void DripMorphPattern_initImpl(Pattern *pat)
{
  DripMorphPattern *self = (DripMorphPattern *)pat;
  MultiLedPattern_doInit(pat);
  // reset the blink timer entirely
  Timer_reset(&self->m_blinkTimer);
  // dops timing
  Timer_addAlarm(&self->m_blinkTimer, self->m_blinkOnDuration);
  Timer_addAlarm(&self->m_blinkTimer, self->m_blinkOffDuration);
  // start the blink timer from the next frame
  Timer_start0(&self->m_blinkTimer);
  // convert current/next colors to HSV
  { RGBColor tmp = Colorset_getNext(&(&self->base.base)->m_colorset); HSVColor_setRGB(&self->m_cur, &tmp); }
  { RGBColor tmp = Colorset_getNext(&(&self->base.base)->m_colorset); HSVColor_setRGB(&self->m_next, &tmp); }
}

void DripMorphPattern_play(Pattern *pat)
{
  DripMorphPattern *self = (DripMorphPattern *)pat;
  switch (Timer_alarm(&self->m_blinkTimer)) {
  case -1: // just break
    break;
  case 0: // turn on the leds for given mapping
    DripMorphPattern_blinkOn(self);
    break;
  case 1: // turn off the leds
    DripMorphPattern_blinkOff(self);
    break;
  }
}

void DripMorphPattern_blinkOn(DripMorphPattern *self)
{
  // if the current hue has reached the next hue
  if (self->m_cur.hue == self->m_next.hue) {
    // copy over the sat/val
    RGBColor_fromHSV(&self->m_cur) = RGBColor_fromHSV(&self->m_next);
    // get the next color and convert to hsv
    { RGBColor tmp = Colorset_getNext(&(&self->base.base)->m_colorset); HSVColor_setRGB(&self->m_next, &tmp); }
  }
  // check which direction is closer for the next hue
  // tbh idk really how this even works anymore but it does
  int sign = ((((self->m_next.hue + 255) - self->m_cur.hue) % 256) <= 128) ? 1 : -1;
  // move hue in the direction of next hue at chosen speed
  // NOTE: if the speed isn't a multiple of the hue values then
  //       it will cause oscillation around the target hue
  //       because it will never reach the target hue and
  //       always over/under shoot
  self->m_cur.hue += self->m_speed * sign;
  // set the target led with the current HSV color
  Leds_setAllEvens(RGBColor_fromHSV(&self->m_cur));
  Leds_setAllOdds(Colorset_cur(&(&self->base.base)->m_colorset));
}

void DripMorphPattern_blinkOff(DripMorphPattern *self)
{
  Leds_clearAll();
}
