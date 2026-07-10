#include "ColorSelect.h"
#include <stdlib.h>

#include "../../VortexEngine.h"

#include "../../Time/TimeControl.h"
#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/Pattern.h"
#include "../../Colors/Colorset.h"
#include "../../Buttons/Button.h"
#include "../../Random/Random.h"
#include "../../Time/Timings.h"
#include "../../Menus/Menus.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#define COL32(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })

static const uint8_t s_sats[4] = { SAT_OPTION_4, SAT_OPTION_3, SAT_OPTION_2, SAT_OPTION_1 };
static const uint8_t s_vals[4] = { VAL_OPTION_4, VAL_OPTION_3, VAL_OPTION_2, VAL_OPTION_1 };

static void showSlotSelection(ColorSelectMenu *self);
static void showSelection(ColorSelectMenu *self, ColorSelectState mode);
static void showFullSet(ColorSelectMenu *self, uint8_t offMs, uint8_t onMs);

extern void VortexEngine_toggleForceSleep(bool enabled);

Menu *ColorSelectMenu_Create(RGBColor col, bool advanced)
{
  ColorSelectMenu *self = (ColorSelectMenu *)malloc(sizeof(ColorSelectMenu));
  if (!self) return NULL;
  Menu_construct(&self->base, col, advanced);
  self->base.vtable = &g_colorSelectMenuVTable;
  self->state = COLOR_SELECT_STATE_PICK_SLOT;
  HSVColor_init(&self->newColor);
  Colorset_init(&self->colorset);
  self->targetSlot = 0;
  self->targetHue1 = 0;
  g_hsv_rgb_alg = HSV_TO_RGB_RAINBOW;
  return &self->base;
}

void ColorSelectMenu_destroy(Menu *self)
{
  (void)self;
  g_hsv_rgb_alg = HSV_TO_RGB_GENERIC;
  VortexEngine_toggleForceSleep(true);
}

bool ColorSelectMenu_init(Menu *self)
{
  if (!Menu_init(self)) {
    return false;
  }
  Mode *cur = Modes_curMode();
  if (Mode_isEmpty(cur)) {
    return false;
  }
  ColorSelectMenu *cs = (ColorSelectMenu *)self;
#if VORTEX_SLIM == 0
  if (Mode_isMultiLed(cur)) {
    Menu_bypassLedSelection(self, MAP_LED(LED_MULTI));
  }
#endif
  cs->state = COLOR_SELECT_STATE_INIT;
  DEBUG_LOG("Entered color select");
  return true;
}

MenuAction ColorSelectMenu_run(Menu *self)
{
  MenuAction result = Menu_run(self);
  if (result != MENU_CONTINUE) {
    return result;
  }
  ColorSelectMenu *cs = (ColorSelectMenu *)self;

  Leds_clearAll();
  switch (cs->state) {
  case COLOR_SELECT_STATE_INIT:
    HSVColor_initHSV(&cs->newColor, 0, 255, 255);
    cs->targetHue1 = 0;
    cs->targetSlot = 0;
    cs->state = COLOR_SELECT_STATE_PICK_SLOT;
    break;
  case COLOR_SELECT_STATE_PICK_SLOT:
    showSlotSelection(cs);
    break;
  case COLOR_SELECT_STATE_PICK_HUE1:
  case COLOR_SELECT_STATE_PICK_HUE2:
  case COLOR_SELECT_STATE_PICK_SAT:
  case COLOR_SELECT_STATE_PICK_VAL:
    showSelection(cs, cs->state);
    break;
  }
  Menus_showSelection(COL32(RGB_WHITE5));
  return MENU_CONTINUE;
}

void ColorSelectMenu_onLedSelected(Menu *self)
{
  ColorSelectMenu *cs = (ColorSelectMenu *)self;
  Mode *cur = Modes_curMode();
  if (self->targetLeds == MAP_LED_ALL) {
    cs->colorset = Mode_getColorset(cur, LED_ALL);
  } else {
    cs->colorset = Mode_getColorset(cur, ledmapGetFirstLed(self->targetLeds));
  }
}

void ColorSelectMenu_onShortClick(Menu *self)
{
  ColorSelectMenu *cs = (ColorSelectMenu *)self;
  self->curSelection++;
  if (cs->state == COLOR_SELECT_STATE_PICK_SLOT) {
    self->curSelection %= (Colorset_numColors(&cs->colorset) + 1 + (Colorset_numColors(&cs->colorset) < MAX_COLOR_SLOTS));
  } else {
    self->curSelection %= 5;
  }
}

void ColorSelectMenu_onLongClick(Menu *self)
{
  ColorSelectMenu *cs = (ColorSelectMenu *)self;
  if (self->curSelection == 4 && cs->state > COLOR_SELECT_STATE_PICK_SLOT) {
    cs->state = (ColorSelectState)(cs->state - 1);
    self->curSelection = (cs->state == COLOR_SELECT_STATE_PICK_SLOT) ? cs->targetSlot : 0;
    return;
  }
  uint8_t numColors = Colorset_numColors(&cs->colorset);
  uint32_t holdDur = Button_holdDuration(g_pButton);
  switch (cs->state) {
  case COLOR_SELECT_STATE_INIT:
    return;
  case COLOR_SELECT_STATE_PICK_SLOT:
    if (self->curSelection == numColors + (numColors < MAX_COLOR_SLOTS)) {
      Mode *cur = Modes_curMode();
      Mode_setColorsetMap(cur, self->targetLeds, &cs->colorset);
      Mode_init(cur);
      Menu_leaveMenu(self, true);
      return;
    }
    if (self->curSelection < numColors &&
        holdDur >= DELETE_THRESHOLD_TICKS &&
        (holdDur % (DELETE_CYCLE_TICKS * 2)) > (DELETE_CYCLE_TICKS)) {
      Colorset_removeColor(&cs->colorset, self->curSelection);
      if (self->curSelection > numColors) {
        self->curSelection--;
      }
      return;
    }
    cs->targetSlot = self->curSelection;
    break;
  case COLOR_SELECT_STATE_PICK_HUE1:
    cs->targetHue1 = self->curSelection;
    cs->newColor.hue = cs->targetHue1 * (255 / 4);
    break;
  case COLOR_SELECT_STATE_PICK_HUE2:
    cs->newColor.hue = (cs->targetHue1 * (255 / 4)) + self->curSelection * (255 / 16);
    break;
  case COLOR_SELECT_STATE_PICK_SAT:
    cs->newColor.sat = s_sats[self->curSelection];
    break;
  case COLOR_SELECT_STATE_PICK_VAL:
  {
    cs->newColor.val = s_vals[self->curSelection];
    RGBColor rgb;
    RGBColor_initFromHSV(&rgb, &cs->newColor);
    Colorset_set(&cs->colorset, cs->targetSlot, rgb);
    self->curSelection = cs->targetSlot;
    cs->state = COLOR_SELECT_STATE_PICK_SLOT;
    return;
  }
  }
  cs->state = (ColorSelectState)(cs->state + 1);
  self->curSelection = 0;
}

// ================== Private rendering ==================

static void showSlotSelection(ColorSelectMenu *self)
{
  uint8_t exitIndex = Colorset_numColors(&self->colorset);
  uint32_t holdDur = Button_holdDuration(g_pButton);
  bool withinNumColors = self->base.curSelection < exitIndex;
  bool holdDurationCheck = Button_isPressed(g_pButton) && holdDur >= DELETE_THRESHOLD_TICKS;
  bool holdDurationModCheck = (holdDur % (DELETE_CYCLE_TICKS * 2)) > DELETE_CYCLE_TICKS;
  RGBColor col = Colorset_get(&self->colorset, self->base.curSelection);
  if (withinNumColors && holdDurationCheck && holdDurationModCheck) {
    Leds_blinkIndex(LED_0, 50, 100, col);
    Leds_breatheIndex(LED_1, 0, holdDur, 22, 255, 180);
  } else if (withinNumColors) {
    if (RGBColor_empty(&col)) {
      Leds_setIndex(LED_0, COL32(RGB_WHITE0));
    }
    Leds_blinkAll(150, 650, col);
  } else if (exitIndex < MAX_COLOR_SLOTS) {
    if (self->base.curSelection == exitIndex) {
      Leds_blinkAll(100, 150, COL32(RGB_WHITE2));
    }
    exitIndex++;
  }
  if (self->base.curSelection == exitIndex) {
    showFullSet(self, 50, 100);
    Leds_setIndex(LED_1, COL32(RGB_GREEN2));
  } else if (self->base.curSelection != Colorset_numColors(&self->colorset) && !holdDurationCheck) {
    Leds_setIndex(LED_1, Button_isPressed(g_pButton) ? COL32(RGB_OFF) : COL32(RGB_WHITE2));
  }
}

static void showSelection(ColorSelectMenu *self, ColorSelectState mode)
{
  if (self->base.curSelection >= 4) {
    Menu_showExit(&self->base);
    return;
  }
  uint32_t now = Time_getCurtime();
  uint8_t hue = self->newColor.hue;
  uint8_t sat = self->newColor.sat;
  uint8_t val = 255;
  switch (mode) {
  default:
    return;
  case COLOR_SELECT_STATE_PICK_HUE1:
    hue = self->base.curSelection * (255 / 4);
    Leds_breatheIndex(LED_0, hue, (now / 2), 22, 255, 180);
    Leds_breatheIndex(LED_1, hue, (now / 2) + 125, 22, 255, 180);
    sat = 255;
    return;
  case COLOR_SELECT_STATE_PICK_HUE2:
    hue = self->targetHue1 * (255 / 4) + (self->base.curSelection * (255 / 16));
    Leds_setIndex(LED_1, COL32(RGB_WHITE0));
    sat = 255;
    break;
  case COLOR_SELECT_STATE_PICK_SAT:
    sat = s_sats[self->base.curSelection];
    Leds_breatheIndexSat(LED_1, hue, (now / 3), 100, 150, 150);
    break;
  case COLOR_SELECT_STATE_PICK_VAL:
    val = s_vals[self->base.curSelection];
    Leds_breatheIndexVal(LED_1, hue, (now / 3), 100, sat, 150);
    break;
  }
  {
    HSVColor hsv;
    HSVColor_initHSV(&hsv, hue, sat, val);
    RGBColor rgb;
    RGBColor_initFromHSV(&rgb, &hsv);
    Leds_setMap(MAP_PAIR_EVENS, rgb);
  }
}

static void showFullSet(ColorSelectMenu *self, uint8_t offMs, uint8_t onMs)
{
  uint8_t numCols = Colorset_numColors(&self->colorset);
  uint32_t offOnMs = MS_TO_TICKS(offMs + onMs);
  if (!numCols || !offOnMs) {
    return;
  }
  uint32_t now = Time_getCurtime();
  if ((now % offOnMs) < MS_TO_TICKS(onMs)) {
    Leds_setAll(Colorset_get(&self->colorset, (now / offOnMs) % numCols));
  }
  Leds_setIndex(LED_1, COL32(RGB_GREEN0));
}

const MenuVTable g_colorSelectMenuVTable = {
  .destroy = ColorSelectMenu_destroy,
  .init = ColorSelectMenu_init,
  .run = ColorSelectMenu_run,
  .onLedSelected = ColorSelectMenu_onLedSelected,
  .onShortClick = ColorSelectMenu_onShortClick,
  .onLongClick = ColorSelectMenu_onLongClick,
  .leaveMenu = Menu_leaveMenu,
};
