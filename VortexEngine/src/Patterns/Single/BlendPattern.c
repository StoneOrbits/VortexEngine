#include "BlendPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

const PatternVTable BlendPattern_vtable = {
  NULL,
  BasicPattern_play,
  BlendPattern_initVirtual,
  Pattern_bindBase,
  BlendPattern_onBlinkOn,
  BasicPattern_onBlinkOff,
  BasicPattern_beginGap,
  BasicPattern_beginDash
};

void BlendPattern_init(BlendPattern *self, const PatternArgs *args)
{
  BasicPattern_init(&self->base, args);
  self->base.base.base.vtable = &BlendPattern_vtable;
  self->blendSpeed = 0;
  self->numFlips = 0;
  RGBColor_init(&self->cur);
  RGBColor_init(&self->next);
  self->flip = 0;
  self->base.base.base.patternID = PATTERN_BLEND;
  REGISTER_ARG(self, self->blendSpeed);
  REGISTER_ARG(self, self->numFlips);
  Pattern_setArgs(&self->base.base.base, args);
}

void BlendPattern_initVirtual(Pattern *base)
{
  BlendPattern *self = (BlendPattern *)base;

  BasicPattern_initVirtual(base);

  self->cur = Colorset_getNext(&base->colorset);
  self->next = Colorset_getNext(&base->colorset);
  self->flip = 0;
}

static void interpolate(uint8_t *current, const uint8_t next, uint8_t blendSpeed)
{
  if (*current < next) {
    uint8_t step = (next - *current) > blendSpeed ? blendSpeed : (next - *current);
    *current += step;
  } else if (*current > next) {
    uint8_t step = (*current - next) > blendSpeed ? blendSpeed : (*current - next);
    *current -= step;
  }
}

void BlendPattern_onBlinkOn(Pattern *base)
{
  BlendPattern *self = (BlendPattern *)base;

  if (RGBColor_equals(&self->cur, &self->next)) {
    self->next = Colorset_getNext(&base->colorset);
  }
  interpolate(&self->cur.red, self->next.red, self->blendSpeed);
  interpolate(&self->cur.green, self->next.green, self->blendSpeed);
  interpolate(&self->cur.blue, self->next.blue, self->blendSpeed);
  RGBColor col = self->cur;

  if (self->flip && self->numFlips) {
    HSVColor hsvCol;
    HSVColor_initFromRGB(&hsvCol, &self->cur);
    hsvCol.hue += (self->flip * (127 / self->numFlips));
    RGBColor_initFromHSV(&col, &hsvCol);
  }
  Leds_setIndex(base->ledPos, col);
  self->flip++;
  if (self->flip > self->numFlips) {
    self->flip = 0;
  }
}
