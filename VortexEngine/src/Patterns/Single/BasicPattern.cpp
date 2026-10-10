#include "BasicPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"
#include "../../Memory/Memory.h"
#include "../PatternBuilder.h"

// uncomment me to print debug labels on the pattern states, this is useful if you
// are debugging a pattern strip from the command line and want to see what state
// the pattern is in each tick of the pattern
//#define DEBUG_BASIC_PATTERN

#ifdef DEBUG_BASIC_PATTERN
#include <stdio.h>
// print out the current state of the pattern
#define PRINT_STATE(state) printState(self, state)
static void printState(const BlinkPattern *self, BlinkPatternState state)
{
  static uint64_t lastPrint = 0;
  if (lastPrint == Time_getCurtime()) return;
  switch (state) {
  case STATE_ON: printf("on  "); break;
  case STATE_OFF: printf("off "); break;
  case STATE_IN_GAP: printf("gap1"); break;
  case STATE_IN_DASH: printf("dash"); break;
  case STATE_IN_GAP2: printf("gap2"); break;
  default: return;
  }
  lastPrint = Time_getCurtime();
}
#else
#define PRINT_STATE(state) // do nothing
#endif

// forward declarations so the vtable can reference every hook below
static void BlinkPattern_onBlinkOnBasic(BlinkPattern *self);
static void BlinkPattern_onBlinkOnBlend(BlinkPattern *self);
void BlinkPattern_onBlinkOn(BlinkPattern *self);
void BlinkPattern_onBlinkOff(BlinkPattern *self);
void BlinkPattern_beginGap(BlinkPattern *self);
void BlinkPattern_beginDash(BlinkPattern *self);
void BlinkPattern_destroy(Pattern *self);
void BlinkPattern_bind(Pattern *self, LedPos pos);
void BlinkPattern_doInit(Pattern *self);
void BlinkPattern_play(Pattern *self);

// the pattern ids that use the colour blending behaviour
bool BlinkPattern_isBlendID(PatternID id)
{
  switch (id) {
  case PATTERN_BLEND:
  case PATTERN_BLENDSTROBE:
  case PATTERN_BLENDSTROBEGAP:
  case PATTERN_COMPLEMENTARY_BLEND:
  case PATTERN_COMPLEMENTARY_BLENDSTROBE:
  case PATTERN_COMPLEMENTARY_BLENDSTROBEGAP:
    return true;
  default:
    return false;
  }
}

void BlinkPattern_init(BlinkPattern *self, PatternID id, const PatternArgs *args)
{
  BlinkPattern_initWithVtable(self, id, args, &BlinkPattern_bvtable);
}

void BlinkPattern_initWithVtable(BlinkPattern *self, PatternID id, const PatternArgs *args, const BlinkPatternVtable *vtable)
{
  Pattern_initBase(&self->base, id, &vtable->base);
  self->bvtable = vtable;

  self->m_onDuration = 0;
  self->m_offDuration = 0;
  self->m_gapDuration = 0;
  self->m_dashDuration = 0;
  self->m_groupSize = 0;
  self->m_blendSpeed = 0;
  self->m_numFlips = 0;
  self->m_groupCounter = 0;
  self->m_state = STATE_BLINK_ON;
  self->m_isBlend = BlinkPattern_isBlendID(id);
  Timer_make(&self->m_blinkTimer);
  self->m_cur = RGBColor_make();
  self->m_next = RGBColor_make();
  self->m_flip = 0;

  REGISTER_ARG((Pattern *)self, BlinkPattern, m_onDuration);
  REGISTER_ARG((Pattern *)self, BlinkPattern, m_offDuration);
  REGISTER_ARG((Pattern *)self, BlinkPattern, m_gapDuration);
  REGISTER_ARG((Pattern *)self, BlinkPattern, m_dashDuration);
  REGISTER_ARG((Pattern *)self, BlinkPattern, m_groupSize);
  // the blend args only exist for the blend ids, which keeps the argument
  // count (and therefore the serialized form and the editor layout) the
  // same as when blend was a separate subclass
  if (self->m_isBlend) {
    REGISTER_ARG((Pattern *)self, BlinkPattern, m_blendSpeed);
    REGISTER_ARG((Pattern *)self, BlinkPattern, m_numFlips);
  }

  Pattern_setArgs(&self->base, args);
}

BlinkPattern *BlinkPattern_make(PatternID id, const PatternArgs *args)
{
  BlinkPattern *self = (BlinkPattern *)PatternBuilder_allocPattern();
  if (!self) {
    return NULL;
  }
  BlinkPattern_init(self, id, args);
  return self;
}

void BlinkPattern_destroy(Pattern *self)
{
  Timer_destroy(&((BlinkPattern *)self)->m_blinkTimer);
}

void BlinkPattern_bind(Pattern *self, LedPos pos)
{
  Pattern_baseBind(self, pos);
}

void BlinkPattern_doInit(Pattern *pat)
{
  BlinkPattern *self = (BlinkPattern *)pat;

  // run base pattern init logic
  Pattern_baseInit(pat);

  // the default state to begin with
  self->m_state = STATE_BLINK_ON;
  // if a dash is present then always start with the dash because
  // it consumes the first color in the colorset
  if (self->m_dashDuration > 0) {
    self->m_state = STATE_BEGIN_DASH;
  }
  // if there's no on duration or dash duration the led is just disabled
  if ((!self->m_onDuration && !self->m_dashDuration) || !Colorset_numColors(&self->base.m_colorset)) {
    self->m_state = STATE_DISABLED;
  }
  self->m_groupCounter = self->m_groupSize ? self->m_groupSize : (Colorset_numColors(&self->base.m_colorset) - (self->m_dashDuration != 0));

  // blend patterns additionally seed the current/target colors and reset
  // the flip counter, this is what BlendPattern::init used to do
  if (self->m_isBlend) {
    self->m_cur = Colorset_getNext(&self->base.m_colorset);
    self->m_next = Colorset_getNext(&self->base.m_colorset);
    self->m_flip = 0;
  }
}

static void BlinkPattern_nextState(BlinkPattern *self, uint8_t timing)
{
  Timer_initN(&self->m_blinkTimer, TIMER_1_ALARM | TIMER_START, timing);
  self->m_state = (BlinkPatternState)(self->m_state + 1);
}

// the merged hook: blend ids blend, every other id does the plain blink.
// keeping both behaviours behind one vtable slot is what lets the former
// BlendPattern subclass disappear without changing any output.
void BlinkPattern_onBlinkOn(BlinkPattern *self)
{
  if (self->m_isBlend) {
    BlinkPattern_onBlinkOnBlend(self);
  } else {
    BlinkPattern_onBlinkOnBasic(self);
  }
}

void BlinkPattern_onBlinkOff(BlinkPattern *self)
{
  PRINT_STATE(STATE_OFF);
  Leds_clearIndex(self->base.m_ledPos);
}

void BlinkPattern_beginGap(BlinkPattern *self)
{
  PRINT_STATE(STATE_IN_GAP);
  Leds_clearIndex(self->base.m_ledPos);
}

void BlinkPattern_beginDash(BlinkPattern *self)
{
  PRINT_STATE(STATE_IN_DASH);
  Leds_setIndex(self->base.m_ledPos, Colorset_getNext(&self->base.m_colorset));
}

// step a single channel towards the target by at most m_blendSpeed
static void BlinkPattern_interpolate(const BlinkPattern *self, uint8_t *current, uint8_t next)
{
  if (*current < next) {
    uint8_t step = (next - *current) > self->m_blendSpeed ? self->m_blendSpeed : (next - *current);
    *current += step;
  } else if (*current > next) {
    uint8_t step = (*current - next) > self->m_blendSpeed ? self->m_blendSpeed : (*current - next);
    *current -= step;
  }
}

// the plain blink behaviour, this is what BasicPattern::onBlinkOn did
static void BlinkPattern_onBlinkOnBasic(BlinkPattern *self)
{
  PRINT_STATE(STATE_ON);
  Leds_setIndex(self->base.m_ledPos, Colorset_getNext(&self->base.m_colorset));
}

// the colour blending behaviour, this is what BlendPattern::onBlinkOn did
static void BlinkPattern_onBlinkOnBlend(BlinkPattern *self)
{
  if (RGBColor_equals(&self->m_cur, &self->m_next)) {
    self->m_next = Colorset_getNext(&self->base.m_colorset);
  }
  BlinkPattern_interpolate(self, &self->m_cur.red, self->m_next.red);
  BlinkPattern_interpolate(self, &self->m_cur.green, self->m_next.green);
  BlinkPattern_interpolate(self, &self->m_cur.blue, self->m_next.blue);
  RGBColor col = self->m_cur;
  // it should be impossible for m_numFlips to be 0 and m_flip to be 1
  // unless the pattern arg is changed mid-play which could happen with
  // and editor or something, so check both for 0
  if (self->m_flip && self->m_numFlips) {
    // convert to hsv
    HSVColor hsvCol = rgb_to_hsv_generic(&self->m_cur);
    // shift the hue by a flip size
    hsvCol.hue += (self->m_flip * (127 / self->m_numFlips));
    // convert the hsv color back to RGB
    col = RGBColor_fromHSV(&hsvCol);
  }
  // set the color
  Leds_setIndex(self->base.m_ledPos, col);
  // increment the flip count
  self->m_flip++;
  // modulate the flip count DO NOT USE MODULO OPERATOR BECAUSE
  // THE FLIP COUNT COULD BE 0 THAT WILL DIVIDE BY ZERO
  if (self->m_flip > self->m_numFlips) {
    self->m_flip = 0;
  }
}

// the single vtable shared by every blink and blend pattern
const BlinkPatternVtable BlinkPattern_bvtable = {
  .base = {
    .destroy = BlinkPattern_destroy,
    .bind = BlinkPattern_bind,
    .init = BlinkPattern_doInit,
    .play = BlinkPattern_play
  },
  .onBlinkOn = BlinkPattern_onBlinkOn,
  .onBlinkOff = BlinkPattern_onBlinkOff,
  .beginGap = BlinkPattern_beginGap,
  .beginDash = BlinkPattern_beginDash
};

void BlinkPattern_play(Pattern *pat)
{
  BlinkPattern *self = (BlinkPattern *)pat;

  // Sometimes the pattern needs to cycle multiple states in a single frame so
  // instead of using a loop or recursion I have just used a simple goto
 replay:

  // its kinda evolving as i go
  switch (self->m_state) {
  case STATE_DISABLED:
    return;
  case STATE_BLINK_ON:
    if (self->m_onDuration > 0) {
      self->bvtable->onBlinkOn(self);
      --self->m_groupCounter;
      BlinkPattern_nextState(self, self->m_onDuration);
      return;
    }
    self->m_state = STATE_BLINK_OFF;
  case STATE_BLINK_OFF:
    // the whole 'should blink off' situation is tricky because we might need
    // to go back to blinking on if our colorset isn't at the end yet
    if (self->m_groupCounter > 0 || (!self->m_gapDuration && !self->m_dashDuration)) {
      if (self->m_offDuration > 0) {
        self->bvtable->onBlinkOff(self);
        BlinkPattern_nextState(self, self->m_offDuration);
        return;
      }
      if (self->m_groupCounter > 0 && self->m_onDuration > 0) {
        self->m_state = STATE_BLINK_ON;
        goto replay;
      }
    }
    self->m_state = STATE_BEGIN_GAP;
  case STATE_BEGIN_GAP:
    self->m_groupCounter = self->m_groupSize ? self->m_groupSize : (Colorset_numColors(&self->base.m_colorset) - (self->m_dashDuration != 0));
    if (self->m_gapDuration > 0) {
      self->bvtable->beginGap(self);
      BlinkPattern_nextState(self, self->m_gapDuration);
      return;
    }
    self->m_state = STATE_BEGIN_DASH;
  case STATE_BEGIN_DASH:
    if (self->m_dashDuration > 0) {
      self->bvtable->beginDash(self);
      BlinkPattern_nextState(self, self->m_dashDuration);
      return;
    }
    self->m_state = STATE_BEGIN_GAP2;
  case STATE_BEGIN_GAP2:
    if (self->m_dashDuration > 0 && self->m_gapDuration > 0) {
      self->bvtable->beginGap(self);
      BlinkPattern_nextState(self, self->m_gapDuration);
      return;
    }
    self->m_state = STATE_BLINK_ON;
    goto replay;
  default:
    break;
  }

  if (Timer_alarm(&self->m_blinkTimer) == ALARM_NONE) {
    // no alarm triggered just stay in current state, return and don't transition states
    PRINT_STATE(self->m_state);
    return;
  }

  // this just transitions the state into the next state, with some edge conditions for
  // transitioning to different states under certain circumstances. Honestly this is
  // a nightmare to read now and idk how to fix it
  if (self->m_state == STATE_IN_GAP2 || (self->m_state == STATE_OFF && self->m_groupCounter > 0)) {
    // this is an edge condition for when in the second gap or off in the non-last off blink
    // then the state actually needs to jump backwards rather than iterate
    self->m_state = self->m_onDuration ? STATE_BLINK_ON : (self->m_dashDuration ? STATE_BEGIN_DASH : STATE_BEGIN_GAP);
  } else if (self->m_state == STATE_OFF && (!self->m_groupCounter || Colorset_numColors(&self->base.m_colorset) == 1)) {
    // this is an edge condition when the state is off but this is the last off blink in the
    // group or there's literally only one color in the group then if there is more blinks
    // left in the group we need to cycle back to blink on instead of to the next state
    self->m_state = (self->m_groupCounter > 0) ? STATE_BLINK_ON : STATE_BEGIN_GAP;
  } else {
    // this is the standard case, iterate to the next state
    self->m_state = (BlinkPatternState)(self->m_state + 1);
  }
  // poor-mans recurse with the new state change (this transitions to a new state within the same tick)
  goto replay;
}
