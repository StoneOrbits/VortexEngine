#include "ColorSelect.h"
#include "../Menus.h"

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
#include "../../Memory/Memory.h"

static void ColorSelect_onLedSelectedHook(void *self);
static void ColorSelect_destroyHook(void *self);
static bool ColorSelect_initHook(void *self);
static MenuAction ColorSelect_runHook(void *self);
static void ColorSelect_onShortClickHook(void *self);
static void ColorSelect_onLongClickHook(void *self);

const MenuVtable ColorSelect_vtable = {
  ColorSelect_initHook,
  ColorSelect_runHook,
  ColorSelect_onLedSelectedHook,
  ColorSelect_onShortClickHook,
  ColorSelect_onLongClickHook,
  NULL,
  ColorSelect_destroyHook,
};

// the options for saturations
static const uint8_t s_sats[4] = {
  SAT_OPTION_4,
  SAT_OPTION_3,
  SAT_OPTION_2,
  SAT_OPTION_1
};

// the options for values
static const uint8_t s_vals[4] = {
  VAL_OPTION_4,
  VAL_OPTION_3,
  VAL_OPTION_2,
  VAL_OPTION_1
};

ColorSelect *ColorSelect_new(const RGBColor *col, bool advanced)
{
  ColorSelect *self = (ColorSelect *)Menus_allocMenu();
  if (!self) {
    return NULL;
  }
  Menu_initBase(&self->m_base, &ColorSelect_vtable);
  RGBColor_setRGB(&self->m_base.m_menuColor, col);
  self->m_base.m_advanced = advanced;
  self->m_state = STATE_PICK_SLOT;
  self->m_newColor = HSVColor_make();
  Colorset_make(&self->m_colorset);
  self->m_targetSlot = 0;
  self->m_targetHue1 = 0;
  // NOTE! Specifically using hsv_to_rgb_rainbow instead of generic because
  // it will generate nicer looking colors and a nicer rainbow to select
  // from than a regular hsv conversion will.  This is because the rainbow
  // function uses a different algorithm to generate the colors that results
  // in a smaller color space with less bright colors. The tradeoff is you lose
  // the bright colors but the rainbow looks a lot better
  g_hsv_rgb_alg = HSV_TO_RGB_RAINBOW;
  return self;
}

void ColorSelect_destroy(ColorSelect *self)
{
  // revert the hsv to rgb algorithm to normal
  g_hsv_rgb_alg = HSV_TO_RGB_GENERIC;
  // make sure force sleep is re-enabled when we leave
  VortexEngine_toggleForceSleep(true);
  Menu_baseDestroy(&self->m_base);
}

bool ColorSelect_init(ColorSelect *self)
{
  Menu *menu = &self->m_base;
  if (!Menu_baseInit(menu)) {
    return false;
  }
  Mode *cur = Modes_curMode();
  if (Mode_isEmpty(cur)) {
    // cannot work with an empty mode
    return false;
  }
#if VORTEX_SLIM == 0
  if (Mode_isMultiLed(cur)) {
    Menu_bypassLedSelection(menu, MAP_LED(LED_MULTI));
  }
#endif
  self->m_state = STATE_INIT;
  DEBUG_LOG("Entered color select");
  return true;
}

MenuAction ColorSelect_run(ColorSelect *self)
{
  Menu *menu = &self->m_base;
  MenuAction result = Menu_runBase(menu);
  if (result != MENU_CONTINUE) {
    return result;
  }

  // all states start with a blank slate
  Leds_clearAll();
  switch (self->m_state) {
  case STATE_INIT:
    // reset all the selection members
    self->m_newColor = HSVColor_makeHSV(0, 255, 255);
    self->m_targetHue1 = 0;
    self->m_targetSlot = 0;
    // move on to picking slot
    self->m_state = STATE_PICK_SLOT;
    break;
  case STATE_PICK_SLOT:
    ColorSelect_showSlotSelection(self);
    break;
  case STATE_PICK_HUE1:
  case STATE_PICK_HUE2:
  case STATE_PICK_SAT:
  case STATE_PICK_VAL:
    ColorSelect_showSelection(self, self->m_state);
    break;
  }
  // show selections
  Menus_showSelection(RGB_WHITE5);
  return MENU_CONTINUE;
}

// callback after the user selects the target led
void ColorSelect_onLedSelected(ColorSelect *self)
{
  Menu *menu = &self->m_base;
  Mode *cur = Modes_curMode();
  // grab the colorset from our selected target led
  Colorset tmp;
  if (menu->m_targetLeds == MAP_LED_ALL) {
    tmp = Mode_getColorset(cur, LED_ANY);
  } else {
    tmp = Mode_getColorset(cur, ledmapGetFirstLed(menu->m_targetLeds));
  }
  Colorset_set(&self->m_colorset, &tmp);
}

void ColorSelect_onShortClick(ColorSelect *self)
{
  Menu *menu = &self->m_base;
  // increment selection
  menu->m_curSelection++;
  if (self->m_state == STATE_PICK_SLOT) {
    menu->m_curSelection %= (Colorset_numColors(&self->m_colorset) + 1 + (Colorset_numColors(&self->m_colorset) < MAX_COLOR_SLOTS));
  } else {
    menu->m_curSelection %= 5;
  }
}

void ColorSelect_onLongClick(ColorSelect *self)
{
  Menu *menu = &self->m_base;
  // if we're on 'exit' and we're on any menu past the slot selection
  if (menu->m_curSelection == 4 && self->m_state > STATE_PICK_SLOT) {
    // move back to the previous selection
    self->m_state = (ColorSelectState)(self->m_state - 1);
    // if we're back to the slot selection then set our position back to the
    // slot we selected, otherwise it's more visually appealing to just return
    // to the first selection on the previous menu
    menu->m_curSelection = (self->m_state == STATE_PICK_SLOT) ? self->m_targetSlot : 0;
    return;
  }
  // reuse these variables lots
  uint8_t numColors = Colorset_numColors(&self->m_colorset);
  uint32_t holdDur = Button_holdDuration(g_pButton);
  switch (self->m_state) {
  case STATE_INIT:
    // nothing
    return;
  case STATE_PICK_SLOT:
    // if the exit is selected then save and exit, this depends on the number
    // of colors in the colorset. If the colorset is full then the exit is
    // just the number of colors (8) but if it's not full then the exit is the
    // number of colors + 1. Example: with 4 cols, cols are on 0, 1, 2, 3,
    // add-color is 4, and exit is 5
    if (menu->m_curSelection == numColors + (numColors < MAX_COLOR_SLOTS)) {
      Mode *cur = Modes_curMode();
      Mode_setColorsetMap(cur, menu->m_targetLeds, &self->m_colorset);
      Mode_initPatterns(cur);
      Menu_leaveMenu(menu, true);
      return;
    }
    // handle if user releases during the delete option
    if (menu->m_curSelection < numColors &&
        holdDur >= DELETE_THRESHOLD_TICKS &&
        (holdDur % (DELETE_CYCLE_TICKS * 2)) > (DELETE_CYCLE_TICKS)) {
      // delete current slot
      Colorset_removeColor(&self->m_colorset, menu->m_curSelection);
      if (menu->m_curSelection > numColors) {
        menu->m_curSelection--;
      }
      return;
    }
    // otherwise store the target slot continue onto the hue selection
    self->m_targetSlot = menu->m_curSelection;
    break;
  case STATE_PICK_HUE1:
    self->m_targetHue1 = menu->m_curSelection;
    self->m_newColor.hue = self->m_targetHue1 * (255 / 4);
    break;
  case STATE_PICK_HUE2:
    self->m_newColor.hue = (self->m_targetHue1 * (255 / 4)) + menu->m_curSelection * (255 / 16);
    break;
  case STATE_PICK_SAT:
    self->m_newColor.sat = s_sats[menu->m_curSelection];
    break;
  case STATE_PICK_VAL:
    // no m_targetVal because you can't go back after this
    self->m_newColor.val = s_vals[menu->m_curSelection];
    // specifically using hsv to rgb rainbow to generate the color
    Colorset_setColor(&self->m_colorset, self->m_targetSlot, RGBColor_fromHSV(&self->m_newColor));
    menu->m_curSelection = self->m_targetSlot;
    self->m_state = STATE_PICK_SLOT;
    return;
  }
  self->m_state = (ColorSelectState)(self->m_state + 1);
  menu->m_curSelection = 0;
}

void ColorSelect_showSlotSelection(ColorSelect *self)
{
  Menu *menu = &self->m_base;
  uint8_t exitIndex = Colorset_numColors(&self->m_colorset);
  uint32_t holdDur = Button_holdDuration(g_pButton);
  bool withinNumColors = menu->m_curSelection < exitIndex;
  bool holdDurationCheck = Button_isPressed(g_pButton) && holdDur >= DELETE_THRESHOLD_TICKS;
  bool holdDurationModCheck = (holdDur % (DELETE_CYCLE_TICKS * 2)) > DELETE_CYCLE_TICKS;
  RGBColor col = Colorset_at(&self->m_colorset, menu->m_curSelection);
  if (withinNumColors && holdDurationCheck && holdDurationModCheck) {
    // breath red for delete slot
    Leds_blinkIndex(LED_0, 50, 100, col);
    Leds_breatheIndex(LED_1, 0, holdDur);
  } else if (withinNumColors) {
    if (RGBColor_empty(&col)) {
      Leds_setIndex(LED_0, RGB_WHITE0);
    }
    Leds_blinkAll(150, 650, col);
  } else if (exitIndex < MAX_COLOR_SLOTS) {
    if (menu->m_curSelection == exitIndex) {
      // blink both leds and blink faster to indicate 'add' new color
      Leds_blinkAll(100, 150, RGB_WHITE2);
    }
    exitIndex++;
  }
  if (menu->m_curSelection == exitIndex) {
    // display the full set
    ColorSelect_showFullSet(self, 50, 100);
    // set LED_1 to green to indicate save and exit
    Leds_setIndex(LED_1, RGB_GREEN2);
    // if not on exitIndex or add new color set LED_1 based on button state
  } else if (menu->m_curSelection != Colorset_numColors(&self->m_colorset) && !holdDurationCheck) {
    Leds_setIndex(LED_1, Button_isPressed(g_pButton) ? RGB_OFF : RGB_WHITE2);
  }
}

void ColorSelect_showSelection(ColorSelect *self, ColorSelectState mode)
{
  Menu *menu = &self->m_base;
  if (menu->m_curSelection >= 4) {
    Menu_showExit(menu);
    return;
  }
  uint32_t now = Time_getCurtime();
  uint8_t hue = self->m_newColor.hue;
  uint8_t sat = self->m_newColor.sat;
  uint8_t val = 255;
  switch (mode) {
  default:
    return;
  case STATE_PICK_HUE1:
    hue = menu->m_curSelection * (255 / 4);
    Leds_breatheIndex(LED_0, hue, (now / 2), 22, 255, 180);
    Leds_breatheIndex(LED_1, hue, (now / 2) + 125, 22, 255, 180);
    // force sat at hue level1
    sat = 255;
    // NOTE: return here
    return;
  case STATE_PICK_HUE2:
    hue = self->m_targetHue1 * (255 / 4) + (menu->m_curSelection * (255 / 16));
    Leds_setIndex(LED_1, RGB_WHITE0);
    // force sat at hue level2
    sat = 255;
    break;
  case STATE_PICK_SAT:
    sat = s_sats[menu->m_curSelection];
    Leds_breatheIndexSat(LED_1, hue, (now / 3), 100, 150, 150);
    break;
  case STATE_PICK_VAL:
    val = s_vals[menu->m_curSelection];
    Leds_breatheIndexVal(LED_1, hue, (now / 3), 100, sat, 150);
    break;
  }
  HSVColor hsv = HSVColor_makeHSV(hue, sat, val);
  Leds_setMap(MAP_PAIR_EVENS, RGBColor_fromHSV(&hsv));
}

void ColorSelect_showFullSet(ColorSelect *self, uint8_t offMs, uint8_t onMs)
{
  uint8_t numCols = Colorset_numColors(&self->m_colorset);
  uint8_t offOnMs = MS_TO_TICKS(offMs + onMs);
  if (!numCols || !offOnMs) {
    return;
  }
  uint32_t now = Time_getCurtime();
  if ((now % offOnMs) < MS_TO_TICKS(onMs)) {
    Leds_setAll(Colorset_get(&self->m_colorset, (now / offOnMs) % numCols));
  }
  Leds_setIndex(LED_1, RGB_GREEN0);
}

static void ColorSelect_onLedSelectedHook(void *self)
{
  ColorSelect_onLedSelected((ColorSelect *)self);
}

static void ColorSelect_onShortClickHook(void *self)
{
  ColorSelect_onShortClick((ColorSelect *)self);
}

static void ColorSelect_onLongClickHook(void *self)
{
  ColorSelect_onLongClick((ColorSelect *)self);
}

static bool ColorSelect_initHook(void *self)
{
  return ColorSelect_init((ColorSelect *)self);
}

static MenuAction ColorSelect_runHook(void *self)
{
  return ColorSelect_run((ColorSelect *)self);
}

static void ColorSelect_destroyHook(void *self)
{
  ColorSelect_destroy((ColorSelect *)self);
}
