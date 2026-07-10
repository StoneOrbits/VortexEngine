#include "Randomizer.h"
#include <stdlib.h>

#include "../../Memory/Memory.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/Pattern.h"
#include "../../Colors/Colorset.h"
#include "../../Random/Random.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Menus/Menus.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#define COL32(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })

static bool reRoll(RandomizerMenu *self);
static void showRandomizationSelect(RandomizerMenu *self);
#if VORTEX_SLIM == 0
static bool reRollMulti(RandomizerMenu *self);
static PatternID rollMultiLedPatternID(Random *ctx);
#endif
static bool reRollSingles(RandomizerMenu *self);
static PatternID rollSingleLedPatternID(Random *ctx);
static Colorset rollColorset(Random *ctx);
static bool rollCustomPattern(Random *ctx, Mode *pMode, LedPos pos);
static void traditionalPattern(Random *ctx, PatternArgs *outArgs);
static void gapPattern(Random *ctx, PatternArgs *outArgs);
static void dashPattern(Random *ctx, PatternArgs *outArgs);
static void crushPattern(Random *ctx, PatternArgs *outArgs);

Menu *RandomizerMenu_Create(RGBColor col, bool advanced)
{
  RandomizerMenu *self = (RandomizerMenu *)malloc(sizeof(RandomizerMenu));
  if (!self) return NULL;
  Menu_construct(&self->base, col, advanced);
  self->base.vtable = &g_randomizerMenuVTable;
  self->lastRandomization = 0;
  self->flags = advanced ? RANDOMIZE_COLORSET : RANDOMIZE_BOTH;
  self->displayHue = 0;
  self->needToSelect = advanced;
  self->autoCycle = false;
  return &self->base;
}

void RandomizerMenu_destroy(Menu *self)
{
  (void)self;
}

bool RandomizerMenu_init(Menu *self)
{
  if (!Menu_init(self)) {
    return false;
  }
  RandomizerMenu *r = (RandomizerMenu *)self;
  Mode *cur = Modes_curMode();
#if VORTEX_SLIM == 0
  if (Mode_hasMultiLed(cur)) {
    ByteStream ledData;
    ByteStream_init(&ledData);
    Pattern *pat = Mode_getPattern(cur, LED_MULTI);
    if (pat) {
      if (!Pattern_serialize(pat, &ledData)) {
        return false;
      }
    }
    Random_seed(&r->multiRandCtx, ByteStream_recalcCRC(&ledData));
  }
#endif
  for (LedPos l = LED_FIRST; l < LED_COUNT; ++l) {
    ByteStream ledData;
    ByteStream_init(&ledData, 0, NULL);
    Pattern *pat = Mode_getPattern(cur, l);
    if (pat) {
      if (!Pattern_serialize(pat, &ledData)) {
        return false;
      }
    }
    Random_seed(&r->singlesRandCtx[l], ByteStream_recalcCRC(&ledData, false));
  }
  DEBUG_LOG("Entered randomizer");
  return true;
}

MenuAction RandomizerMenu_run(Menu *self)
{
  MenuAction result = Menu_run(self);
  if (result != MENU_CONTINUE) {
    return result;
  }
  RandomizerMenu *r = (RandomizerMenu *)self;
  if (r->needToSelect) {
    showRandomizationSelect(r);
    return MENU_CONTINUE;
  }
#if VORTEX_SLIM == 0
  if (Mode_isMultiLed(&self->previewMode) && self->targetLeds != MAP_LED(LED_MULTI)) {
    PatternID newID = (PatternID)((Mode_getPatternID(&self->previewMode, LED_ALL) - PATTERN_MULTI_FIRST) % PATTERN_SINGLE_COUNT);
    if (newID == PATTERN_SOLID) ++newID;
    Mode_setPattern(&self->previewMode, newID, LED_ALL, NULL, NULL);
    Mode_init(&self->previewMode);
  }
#endif
  uint32_t now = Time_getCurtime();
  if (r->autoCycle && (r->lastRandomization + AUTO_RANDOM_DELAY_TICKS < now)) {
    r->lastRandomization = now;
    reRoll(r);
  }
  Mode_play(&self->previewMode);
  Menus_showSelection(COL32(RGB_WHITE5));
  return MENU_CONTINUE;
}

void RandomizerMenu_onShortClick(Menu *self)
{
  RandomizerMenu *r = (RandomizerMenu *)self;
  if (r->needToSelect) {
    if (r->flags == RANDOMIZE_BOTH) {
      r->flags = RANDOMIZE_COLORSET;
    } else {
      r->flags = (RandomizeFlags)(r->flags + 1);
    }
    return;
  }
  if (r->autoCycle || Button_onConsecutivePresses(g_pButton, AUTO_CYCLE_RANDOMIZER_CLICKS)) {
    r->autoCycle = !r->autoCycle;
    Leds_holdAll(r->autoCycle ? COL32(RGB_GREEN) : COL32(RGB_RED));
    return;
  }
  reRoll(r);
}

void RandomizerMenu_onLongClick(Menu *self)
{
  RandomizerMenu *r = (RandomizerMenu *)self;
  if (r->needToSelect) {
    r->needToSelect = false;
    return;
  }
  Modes_updateCurMode(&self->previewMode);
  Menu_leaveMenu(self, true);
}

static bool reRoll(RandomizerMenu *self)
{
#if VORTEX_SLIM == 0
  if (self->base.targetLeds == MAP_LED(LED_MULTI)) {
    if (!reRollMulti(self)) {
      return false;
    }
  } else if (!reRollSingles(self)) {
    return false;
  }
#else
  if (!reRollSingles(self)) {
    return false;
  }
#endif
  Mode_init(&self->base.previewMode);
  return true;
}

static void showRandomizationSelect(RandomizerMenu *self)
{
  HSVColor hsv;
  HSVColor_initHSV(&hsv, self->displayHue++, (self->flags & RANDOMIZE_COLORSET) * 255, 84);
  RGBColor rgb;
  RGBColor_initFromHSV(&rgb, &hsv);
  Leds_setAll(rgb);
  if (self->flags & RANDOMIZE_PATTERN) {
    { RGBColor _c = {0,0,0}; Leds_blinkAll(DOPS_ON_DURATION, DOPS_OFF_DURATION, _c); }
  }
  Leds_setIndex(LED_1, Button_isPressed(g_pButton) ? COL32(RGB_OFF) : COL32(RGB_WHITE1));
  Menus_showSelection(COL32(RGB_WHITE5));
}

#if VORTEX_SLIM == 0
static bool reRollMulti(RandomizerMenu *self)
{
  if (self->flags & RANDOMIZE_PATTERN) {
    if (!Mode_setPattern(&self->base.previewMode, rollMultiLedPatternID(&self->multiRandCtx), LED_MULTI, NULL, NULL)) {
      ERROR_LOG("Failed to select pattern");
      return false;
    }
  }
  if (self->flags & RANDOMIZE_COLORSET) {
    Colorset cs = rollColorset(&self->multiRandCtx);
    if (!Mode_setColorset(&self->base.previewMode, &cs, LED_MULTI)) {
      ERROR_LOG("Failed to roll new colorset");
      return false;
    }
  }
  return true;
}

static PatternID rollMultiLedPatternID(Random *ctx)
{
  return (PatternID)Random_next8(ctx, PATTERN_MULTI_FIRST, PATTERN_MULTI_LAST);
}
#endif

static bool reRollSingles(RandomizerMenu *self)
{
  MAP_FOREACH_LED(self->base.targetLeds) {
    Random *ctx = &self->singlesRandCtx[pos];
    if (self->flags & RANDOMIZE_PATTERN) {
      if (self->base.advanced) {
        if (!rollCustomPattern(ctx, &self->base.previewMode, pos)) {
          ERROR_LOG("Failed to roll custom pattern");
          return false;
        }
      } else {
        if (!Mode_setPattern(&self->base.previewMode, rollSingleLedPatternID(ctx), pos, NULL, NULL)) {
          ERROR_LOG("Failed to select pattern");
          return false;
        }
      }
    }
    if (self->flags & RANDOMIZE_COLORSET) {
      Colorset cs = rollColorset(ctx);
      if (!Mode_setColorset(&self->base.previewMode, &cs, pos)) {
        ERROR_LOG("Failed to roll new colorset");
        return false;
      }
    }
  }
  return true;
}

static PatternID rollSingleLedPatternID(Random *ctx)
{
  PatternID newPat;
  do {
    newPat = (PatternID)Random_next8(ctx, PATTERN_SINGLE_FIRST, PATTERN_SINGLE_LAST);
  } while (newPat == PATTERN_SOLID || newPat == PATTERN_RIBBON || newPat == PATTERN_MINIRIBBON);
  return newPat;
}

static Colorset rollColorset(Random *ctx)
{
  Colorset randomSet;
  Colorset_init(&randomSet);
  uint8_t randType = Random_next8(ctx, 0, 8);
  switch (randType) {
  default:
  case 0:
    Colorset_randomize(&randomSet, ctx, 0);
    break;
  case 1:
    Colorset_randomizeColors(&randomSet, ctx, 0, COLOR_MODE_THEORY);
    break;
  case 2:
    Colorset_randomizeColors(&randomSet, ctx, 0, COLOR_MODE_MONOCHROMATIC);
    break;
  case 3:
    Colorset_randomizeColors2(&randomSet, ctx, COLOR_MODE2_DOUBLE_SPLIT_COMPLIMENTARY);
    break;
  case 4:
    Colorset_randomizeColors2(&randomSet, ctx, COLOR_MODE2_TETRADIC);
    break;
  case 5:
    Colorset_randomize(&randomSet, ctx, 1);
    break;
  case 6:
    Colorset_randomizeColors(&randomSet, ctx, 0, COLOR_MODE_EVENLY_SPACED);
    break;
  case 7:
    Colorset_randomizeColors(&randomSet, ctx, 2, COLOR_MODE_EVENLY_SPACED);
    break;
  case 8:
    Colorset_randomizeColors(&randomSet, ctx, 3, COLOR_MODE_EVENLY_SPACED);
    break;
  }
  if (Colorset_numColors(&randomSet) <= 4 && Random_next8(ctx, 0, 1) != 0) {
    int16_t startingNumColors = Colorset_numColors(&randomSet);
    for (int16_t i = 0; i < startingNumColors - 1; ++i) {
      Colorset_addColor(&randomSet, Colorset_get(&randomSet, startingNumColors - (i + 2)));
    }
  }
  return randomSet;
}

static bool rollCustomPattern(Random *ctx, Mode *pMode, LedPos pos)
{
  PatternArgs args;
  PatternArgs_init(&args);
  uint8_t patternType = Random_next8(ctx, 0, 3);
  switch (patternType) {
  default:
  case 0:
    traditionalPattern(ctx, &args);
    break;
  case 1:
    gapPattern(ctx, &args);
    break;
  case 2:
    dashPattern(ctx, &args);
    break;
  case 3:
    crushPattern(ctx, &args);
    break;
  }
  PatternID newPat = PATTERN_STROBE;
  if (!Random_next8(ctx, 0, 3)) {
    newPat = PATTERN_BLEND;
    args.arg6 = Random_next8(ctx, 1, 10);
    if (!Random_next8(ctx, 0, 2)) {
      args.arg7 = Random_next8(ctx, 0, 3);
    }
    args.numArgs = 7;
  }
  return Mode_setPattern(pMode, newPat, pos, &args, NULL);
}

static void traditionalPattern(Random *ctx, PatternArgs *outArgs)
{
  uint8_t off = Random_next8(ctx, 8, 60);
  uint8_t on = Random_next8(ctx, 1, 20);
  PatternArgs_init2(outArgs, on, off);
}

static void gapPattern(Random *ctx, PatternArgs *outArgs)
{
  uint8_t gap = Random_next8(ctx, 40, 100);
  uint8_t off = Random_next8(ctx, 0, 6);
  uint8_t on = Random_next8(ctx, 1, 10);
  PatternArgs_init3(outArgs, on, off, gap);
}

static void dashPattern(Random *ctx, PatternArgs *outArgs)
{
  uint8_t dash = Random_next8(ctx, 20, 30);
  uint8_t gap = Random_next8(ctx, 20, 30);
  uint8_t off = Random_next8(ctx, 0, 10);
  uint8_t on = Random_next8(ctx, 1, 10);
  PatternArgs_init4(outArgs, on, off, gap, dash);
}

static void crushPattern(Random *ctx, PatternArgs *outArgs)
{
  uint8_t group = Random_next8(ctx, 0, 8);
  uint8_t dash = 0;
  uint8_t gap = Random_next8(ctx, 20, 40);
  uint8_t off = Random_next8(ctx, 0, 10);
  uint8_t on = Random_next8(ctx, 1, 10);
  PatternArgs_init5(outArgs, on, off, gap, dash, group);
}

const MenuVTable g_randomizerMenuVTable = {
  .destroy = RandomizerMenu_destroy,
  .init = RandomizerMenu_init,
  .run = RandomizerMenu_run,
  .onLedSelected = Menu_onLedSelected,
  .onShortClick = RandomizerMenu_onShortClick,
  .onLongClick = RandomizerMenu_onLongClick,
  .leaveMenu = Menu_leaveMenu,
};
