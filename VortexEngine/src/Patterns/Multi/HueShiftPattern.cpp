#include "HueShiftPattern.h"

#include <string.h>
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const PatternVtable HueShiftPattern_vtable = {
  .destroy = HueShiftPattern_destroy,
  .bind = MultiLedPattern_bind,
  .init = HueShiftPattern_initImpl,
  .play = HueShiftPattern_play
};

void HueShiftPattern_init(HueShiftPattern *self, const PatternArgs *args)
{
  MultiLedPattern_init(&self->base, args, &HueShiftPattern_vtable);
  self->m_blinkOnDuration = 0;
  self->m_blinkOffDuration = 0;
  self->m_blendDelay = 0;
  self->m_delayCounter = 0;
  Timer_make(&self->m_blinkTimer);
  self->m_cur = HSVColor_make();
  self->m_next = HSVColor_make();
  Pattern_setPatternID((&self->base.base), PATTERN_HUE_SCROLL);
  REGISTER_ARG((&self->base.base), HueShiftPattern, m_blinkOnDuration);
  REGISTER_ARG((&self->base.base), HueShiftPattern, m_blinkOffDuration);
  REGISTER_ARG((&self->base.base), HueShiftPattern, m_blendDelay);
  Pattern_setArgs((&self->base.base), args);
}

void HueShiftPattern_destroy(Pattern *pat)
{
  HueShiftPattern *self = (HueShiftPattern *)pat;
  (void)self;
}

void HueShiftPattern_initImpl(Pattern *pat)
{
  HueShiftPattern *self = (HueShiftPattern *)pat;
  MultiLedPattern_doInit(pat);
  { RGBColor tmp = Colorset_getNext(&(&self->base.base)->m_colorset); HSVColor_setRGB(&self->m_cur, &tmp); }
  { RGBColor tmp = Colorset_getNext(&(&self->base.base)->m_colorset); HSVColor_setRGB(&self->m_next, &tmp); }
  // reset the blink timer entirely
  Timer_reset(&self->m_blinkTimer);
  // dops timing
  Timer_addAlarm(&self->m_blinkTimer, self->m_blinkOnDuration);
  Timer_addAlarm(&self->m_blinkTimer, self->m_blinkOffDuration);
  // start the blink timer from the next frame
  Timer_start0(&self->m_blinkTimer);
}

void HueShiftPattern_play(Pattern *pat)
{
  HueShiftPattern *self = (HueShiftPattern *)pat;
  switch (Timer_alarm(&self->m_blinkTimer)) {
  case 0: // turn on the leds for given mapping
    break;
  case 1: // turn off the leds
    Leds_clearAll();
  case -1: // just break and still run post-step
    return;
  }
  // if the current hue has reached the next hue
  if (self->m_cur.hue == self->m_next.hue) {
    // copy over the sat/val
    self->m_cur = self->m_next;
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
  // only increment every blendDelay times
  ++self->m_delayCounter;
  if (self->m_delayCounter >= self->m_blendDelay) {
    self->m_delayCounter = 0;
    self->m_cur.hue += sign;
  }
  HSVColor showColor = HSVColor_makeHSV(self->m_cur.hue, 255, 255);
  // set the target led with the current HSV color
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    Leds_setIndex(pos, hsv_to_rgb_generic(&showColor));
    showColor.hue = (showColor.hue + 5) % 256;
  }
}
