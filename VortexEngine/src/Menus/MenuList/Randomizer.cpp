#include "Randomizer.h"
#include "../Menus.h"

#include "../../Memory/Memory.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/Pattern.h"
#include "../../Patterns/PatternArgs.h"
#include "../../Colors/Colorset.h"
#include "../../Random/Random.h"
#include "../../Serial/ByteStream.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Menus/Menus.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

static void Randomizer_destroyHook(void *self);
static bool Randomizer_initHook(void *self);
static MenuAction Randomizer_runHook(void *self);
static void Randomizer_onShortClickHook(void *self);
static void Randomizer_onLongClickHook(void *self);

const MenuVtable Randomizer_vtable = {
  Randomizer_initHook,
  Randomizer_runHook,
  NULL,
  Randomizer_onShortClickHook,
  Randomizer_onLongClickHook,
  NULL,
  Randomizer_destroyHook,
};

Randomizer *Randomizer_new(const RGBColor *col, bool advanced)
{
  Randomizer *self = (Randomizer *)Menus_allocMenu();
  if (!self) {
    return NULL;
  }
  Menu_initBase(&self->m_base, &Randomizer_vtable);
  RGBColor_setRGB(&self->m_base.m_menuColor, col);
  self->m_base.m_advanced = advanced;
  self->m_lastRandomization = 0;
  self->m_flags = advanced ? RANDOMIZE_COLORSET : RANDOMIZE_BOTH;
  self->m_displayHue = 0;
  self->m_needToSelect = advanced;
  self->m_autoCycle = false;
  return self;
}

void Randomizer_destroy(Randomizer *self)
{
  Menu_baseDestroy(&self->m_base);
}

bool Randomizer_init(Randomizer *self)
{
  Menu *menu = &self->m_base;
  if (!Menu_baseInit(menu)) {
    return false;
  }
  // grab the multi ld pattern colorset crc if it's present
  Mode *cur = Modes_curMode();
#if VORTEX_SLIM == 0
  if (Mode_hasMultiLed(cur)) {
    ByteStream ledData;
    ByteStream_make(&ledData);
    const Pattern *pat = Mode_getPattern(cur, LED_MULTI);
    if (pat) {
      if (!Pattern_serialize(pat, &ledData)) {
        ByteStream_destroy(&ledData);
        return false;
      }
    }
    Random_seed(&self->m_multiRandCtx, ByteStream_recalcCRC(&ledData, false));
    ByteStream_destroy(&ledData);
  }
#endif
  // initialize the randomseed of each led with the
  // CRC of the colorset on the respective LED
  for (LedPos l = LED_FIRST; l < LED_COUNT; ++l) {
    ByteStream ledData;
    ByteStream_make(&ledData);
    const Pattern *pat = Mode_getPattern(cur, l);
    if (pat) {
      if (!Pattern_serialize(pat, &ledData)) {
        ByteStream_destroy(&ledData);
        return false;
      }
    }
    Random_seed(&self->m_singlesRandCtx[l], ByteStream_recalcCRC(&ledData, false));
    ByteStream_destroy(&ledData);
  }
  DEBUG_LOG("Entered randomizer");
  return true;
}

MenuAction Randomizer_run(Randomizer *self)
{
  Menu *menu = &self->m_base;
  // run the base menu logic
  MenuAction result = Menu_runBase(menu);
  if (result != MENU_CONTINUE) {
    return result;
  }
  // if the ranomization flags haven't been set yet just show a selection
  if (self->m_needToSelect) {
    // display the randomization selection menu
    Randomizer_showRandomizationSelect(self);
    return MENU_CONTINUE;
  }
#if VORTEX_SLIM == 0
  // if they are trying to randomize a multi-led pattern just convert
  // the pattern to all singles with the same colorset upon entry
  if (Mode_isMultiLed(&menu->m_previewMode) && menu->m_targetLeds != MAP_LED(LED_MULTI)) {
    // convert the pattern to a single led pattern, this will map
    // all multi led patterns to single led patterns using modulo
    // so no matter which multi-led pattern they have selected it
    // will convert to a single led pattern of some kind
    PatternID newID = (PatternID)((Mode_getPatternID(&menu->m_previewMode, LED_ANY) - PATTERN_MULTI_FIRST) % PATTERN_SINGLE_COUNT);
    // solid sucks
    if (newID == PATTERN_SOLID) ++newID;
    Mode_setPattern(&menu->m_previewMode, newID, LED_ANY, NULL, NULL);
    Mode_initPatterns(&menu->m_previewMode);
  }
#endif
  uint32_t now = Time_getCurtime();
  if (self->m_autoCycle && (self->m_lastRandomization + AUTO_RANDOM_DELAY_TICKS < now)) {
    self->m_lastRandomization = now;
    Randomizer_reRoll(self);
  }
  // display the randomized mode
  Mode_play(&menu->m_previewMode);
  // show the selection
  Menus_showSelection(RGB_WHITE5);
  // return true to continue staying in randomizer menu
  return MENU_CONTINUE;
}

void Randomizer_onShortClick(Randomizer *self)
{
  if (self->m_needToSelect) {
    if (self->m_flags == RANDOMIZE_BOTH) {
      self->m_flags = RANDOMIZE_COLORSET;
    } else {
      self->m_flags = (uint8_t)(self->m_flags + 1);
    }
    return;
  }
  // if the user fast-clicks 3 times then toggle automode
  if (self->m_autoCycle || Button_onConsecutivePresses(g_pButton, AUTO_CYCLE_RANDOMIZER_CLICKS)) {
    // toggle the auto cycle flag
    self->m_autoCycle = !self->m_autoCycle;
    // display a quick flash of either green or red to indicate whether auto mode is on or not
    Leds_holdAll(self->m_autoCycle ? RGB_GREEN : RGB_RED);
    return;
  }
  // shortClick re-roll the randomization
  Randomizer_reRoll(self);
}

void Randomizer_onLongClick(Randomizer *self)
{
  Menu *menu = &self->m_base;
  // if done the randomization selection part
  if (self->m_needToSelect) {
    // we are complete the randomization selection stage
    self->m_needToSelect = false;
    return;
  }
  // update the current mode with the new randomized mode
  Modes_updateCurMode(&menu->m_previewMode);
  // then done here, save if the mode was different
  Menu_leaveMenu(menu, true);
}

bool Randomizer_reRoll(Randomizer *self)
{
  Menu *menu = &self->m_base;
#if VORTEX_SLIM == 0
  if (menu->m_targetLeds == MAP_LED(LED_MULTI)) {
    if (!Randomizer_reRollMulti(self)) {
      return false;
    }
  } else if (!Randomizer_reRollSingles(self)) {
    return false;
  }
#else
  if (!Randomizer_reRollSingles(self)) {
    return false;
  }
#endif
  // initialize the mode with the new pattern and colorset
  Mode_initPatterns(&menu->m_previewMode);
  return true;
}

void Randomizer_showRandomizationSelect(Randomizer *self)
{
  HSVColor hsv = HSVColor_makeHSV(self->m_displayHue++, (self->m_flags & RANDOMIZE_COLORSET) * 255, 84);
  // show iterating rainbow if they are randomizing color, otherwise 0 sat if they
  // are only randomizing the pattern
  Leds_setAll(RGBColor_fromHSV(&hsv));
  if (self->m_flags & RANDOMIZE_PATTERN) {
    // this is blinking the light to off so the params are switched but still effectively correct
    Leds_blinkAll(DOPS_ON_DURATION, DOPS_OFF_DURATION, RGB_OFF);
  }
  // indicate on the 2nd led whether the button is pressed
  Leds_setIndex(LED_1, Button_isPressed(g_pButton) ? RGB_OFF : RGB_WHITE1);
  // render the click selection blink
  Menus_showSelection(RGB_WHITE5);
}

#if VORTEX_SLIM == 0
bool Randomizer_reRollMulti(Randomizer *self)
{
  Menu *menu = &self->m_base;
  if (self->m_flags & RANDOMIZE_PATTERN) {
    // TODO: Roll custom multi pattern?
    //if (m_advanced) {
    //  if (!rollCustomPattern(ctx, &m_previewMode, pos)) {
    //    ERROR_LOG("Failed to roll custom pattern");
    //    return false;
    //  }
    //}
    if (!Mode_setPattern(&menu->m_previewMode, Randomizer_rollMultiLedPatternID(self, &self->m_multiRandCtx), LED_MULTI, NULL, NULL)) {
      ERROR_LOG("Failed to select pattern");
      return false;
    }
  }
  if (self->m_flags & RANDOMIZE_COLORSET) {
    Colorset set = Randomizer_rollColorset(self, &self->m_multiRandCtx);
    if (!Mode_setColorset(&menu->m_previewMode, &set, LED_MULTI)) {
      ERROR_LOG("Failed to roll new colorset");
      return false;
    }
  }
  return true;
}

PatternID Randomizer_rollMultiLedPatternID(Randomizer *self, Random *ctx)
{
  return (PatternID)Random_next8(ctx, PATTERN_MULTI_FIRST, PATTERN_MULTI_LAST);
}
#endif

bool Randomizer_reRollSingles(Randomizer *self)
{
  Menu *menu = &self->m_base;
  // re-roll each led position with it's respective random context
  MAP_FOREACH_LED(menu->m_targetLeds) {
    Random *ctx = &self->m_singlesRandCtx[pos];
    if (self->m_flags & RANDOMIZE_PATTERN) {
      // in advanced mode, when not randomizing the multi position, use a
      // special function to randomize totally custom led pattern timings
      if (menu->m_advanced) {
        if (!Randomizer_rollCustomPattern(self, ctx, &menu->m_previewMode, pos)) {
          ERROR_LOG("Failed to roll custom pattern");
          return false;
        }
      } else {
        if (!Mode_setPattern(&menu->m_previewMode, Randomizer_rollSingleLedPatternID(self, ctx), pos, NULL, NULL)) {
          ERROR_LOG("Failed to select pattern");
          return false;
        }
      }
    }
    if (self->m_flags & RANDOMIZE_COLORSET) {
      Colorset set = Randomizer_rollColorset(self, ctx);
      if (!Mode_setColorset(&menu->m_previewMode, &set, pos)) {
        ERROR_LOG("Failed to roll new colorset");
        return false;
      }
    }
  }
  return true;
}

PatternID Randomizer_rollSingleLedPatternID(Randomizer *self, Random *ctx)
{
  PatternID newPat;
  // the random range begin/end
  do {
    // continuously re-randomize the pattern so we don't get undesirable patterns
    newPat = (PatternID)Random_next8(ctx, PATTERN_SINGLE_FIRST, PATTERN_SINGLE_LAST);
  } while (newPat == PATTERN_SOLID || newPat == PATTERN_RIBBON || newPat == PATTERN_MINIRIBBON);
  return newPat;
}

Colorset Randomizer_rollColorset(Randomizer *self, Random *ctx)
{
  Colorset randomSet;
  Colorset_make(&randomSet);
  // pick a random type of randomizer to use then use
  // the randomizer to generate a random colorset
  uint8_t randType = Random_next8(ctx, 0, 8);
  switch (randType) {
  default:
  case 0:
    Colorset_randomize(&randomSet, ctx, 0);
    break;
  case 1:
    Colorset_randomizeColors(&randomSet, ctx, 0, THEORY);
    break;
  case 2:
    Colorset_randomizeColors(&randomSet, ctx, 0, MONOCHROMATIC);
    break;
  case 3:
    Colorset_randomizeColors2(&randomSet, ctx, DOUBLE_SPLIT_COMPLIMENTARY);
    break;
  case 4:
    Colorset_randomizeColors2(&randomSet, ctx, TETRADIC);
    break;
  case 5:
    Colorset_randomize(&randomSet, ctx, 1);
    break;
  case 6:
    Colorset_randomizeColors(&randomSet, ctx, 0, EVENLY_SPACED);
    break;
  case 7:
    Colorset_randomizeColors(&randomSet, ctx, 2, EVENLY_SPACED);
    break;
  case 8:
    Colorset_randomizeColors(&randomSet, ctx, 3, EVENLY_SPACED);
    break;
  }

  // take the randomized colorset and repeate colors to make an additional pattern
  if (Colorset_numColors(&randomSet) <= 4 && Random_next8(ctx, 0, 1) != 0) {
    // using uint8_t here actually costs more bytes than int16_t
    int16_t startingNumColors = Colorset_numColors(&randomSet);
    for (int16_t i = 0; i < startingNumColors - 1; ++i) {
      // add a duplicate of each color in the colorset to in reverse
      Colorset_addColor(&randomSet, Colorset_get(&randomSet, startingNumColors - (i + 2)));
    }
  }
  return randomSet;
}

bool Randomizer_rollCustomPattern(Randomizer *self, Random *ctx, Mode *pMode, LedPos pos)
{
  PatternArgs args;
  PatternArgs_init0(&args);
  // pick a random type of randomizer to use then use
  // the randomizer to generate a random pattern
  uint8_t patternType = Random_next8(ctx, 0, 3);
  switch (patternType) {
  default:
  case 0:
    Randomizer_traditionalPattern(ctx, &args);
    break;
  case 1:
    Randomizer_gapPattern(ctx, &args);
    break;
  case 2:
    Randomizer_dashPattern(ctx, &args);
    break;
  case 3:
    Randomizer_crushPattern(ctx, &args);
    break;
  }
  PatternID newPat = PATTERN_STROBE;
  // 1/3 chance to roll a blend pattern instead which will animate between
  // colors instead of blinking each color in the set
  if (!Random_next8(ctx, 0, 3)) {
    newPat = PATTERN_BLEND;
    // set the blend speed
    args.arg6 = Random_next8(ctx, 1, 10);
    // 1/2 chance to make the blend a flipping blend of some kind
    if (!Random_next8(ctx, 0, 2)) {
      // this is the number of blinks to a complementary color
      args.arg7 = Random_next8(ctx, 0, 3);
    }
    // up to arg7 is filled now
    args.numArgs = 7;
  }
  return Mode_setPattern(pMode, newPat, pos, &args, NULL);
}

void Randomizer_traditionalPattern(Random *ctx, PatternArgs *outArgs)
{
  // call next8 explicitly in this order because the order they
   // are called is undefined when called as parameters to another function.
   // ex: f(a,b,c) may call in the order a,b,c or c,b,a depending on compiler.
   // So different compilers may produce different results,
   // but like this it is explicit
  uint8_t off = Random_next8(ctx, 8, 60);   // off duration 0 -> 60
  uint8_t on = Random_next8(ctx, 1, 20);    // on duration 1 -> 20
  PatternArgs_init(outArgs, on, off);
}

void Randomizer_gapPattern(Random *ctx, PatternArgs *outArgs)
{
  // call next8 explicitly in this order because the order they
  // are called is undefined when called as parameters to another function.
  // ex: f(a,b,c) may call in the order a,b,c or c,b,a depending on compiler.
  // So different compilers may produce different results,
  // but like this it is explicit
  uint8_t gap = Random_next8(ctx, 40, 100); // gap duration 40 -> 100
  uint8_t off = Random_next8(ctx, 0, 6);    // off duration 0 -> 6
  uint8_t on = Random_next8(ctx, 1, 10);    // on duration 1 -> 10
  PatternArgs_init(outArgs, on, off, gap);
}

void Randomizer_dashPattern(Random *ctx, PatternArgs *outArgs)
{
  // call next8 explicitly in this order because the order they
  // are called is undefined when called as parameters to another function.
  // ex: f(a,b,c) may call in the order a,b,c or c,b,a depending on compiler.
  // So different compilers may produce different results,
  // but like this it is explicit
  uint8_t dash = Random_next8(ctx, 20, 30); // dash duration 20 -> 30
  uint8_t gap = Random_next8(ctx, 20, 30);  // need gap 20 -> 30
  uint8_t off = Random_next8(ctx, 0, 10);   // off duration 0 -> 10
  uint8_t on = Random_next8(ctx, 1, 10);    // on duration 1 -> 10
  PatternArgs_init(outArgs, on, off, gap, dash);
}

void Randomizer_crushPattern(Random *ctx, PatternArgs *outArgs)
{
  // call next8 explicitly in this order because the order they
  // are called is undefined when called as parameters to another function.
  // ex: f(a,b,c) may call in the order a,b,c or c,b,a depending on compiler.
  // So different compilers may produce different results,
  // but like this it is explicit
  uint8_t group = Random_next8(ctx, 0, 8);  // groupsize 0 to 8
  uint8_t dash = 0;                 // dash 0
  uint8_t gap = Random_next8(ctx, 20, 40);  // need gap 20 -> 40
  uint8_t off = Random_next8(ctx, 0, 10);   // off duration 0 -> 5
  uint8_t on = Random_next8(ctx, 1, 10);    // on duration 1 -> 10
  PatternArgs_init(outArgs, on, off, gap, dash, group);
}

static void Randomizer_onShortClickHook(void *self)
{
  Randomizer_onShortClick((Randomizer *)self);
}

static void Randomizer_onLongClickHook(void *self)
{
  Randomizer_onLongClick((Randomizer *)self);
}

static bool Randomizer_initHook(void *self)
{
  return Randomizer_init((Randomizer *)self);
}

static MenuAction Randomizer_runHook(void *self)
{
  return Randomizer_run((Randomizer *)self);
}

static void Randomizer_destroyHook(void *self)
{
  Randomizer_destroy((Randomizer *)self);
}
