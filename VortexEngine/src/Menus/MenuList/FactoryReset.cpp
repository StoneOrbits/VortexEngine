#include "FactoryReset.h"
#include "../Menus.h"

#include "../../VortexEngine.h"
#include "../../Modes/DefaultModes.h"
#include "../../Time/TimeControl.h"
#include "../../Patterns/Pattern.h"
#include "../../Buttons/Button.h"
#include "../../Time/Timings.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"

#include "../../VortexConfig.h"

// the derived hooks, dispatched through the base Menu vtable
static void FactoryReset_destroyHook(void *self);
static bool FactoryReset_initHook(void *self);
static MenuAction FactoryReset_runHook(void *self);
static void FactoryReset_onShortClickHook(void *self);
static void FactoryReset_onLongClickHook(void *self);

const MenuVtable FactoryReset_vtable = {
  FactoryReset_initHook,
  FactoryReset_runHook,
  NULL,
  FactoryReset_onShortClickHook,
  FactoryReset_onLongClickHook,
  NULL,
  FactoryReset_destroyHook,
};

FactoryReset *FactoryReset_new(const RGBColor *col, bool advanced)
{
  FactoryReset *self = (FactoryReset *)Menus_allocMenu();
  if (!self) {
    return NULL;
  }
  Menu_initBase(&self->m_base, &FactoryReset_vtable);
  RGBColor_setRGB(&self->m_base.m_menuColor, col);
  self->m_base.m_advanced = advanced;
  return self;
}

void FactoryReset_destroy(FactoryReset *self)
{
  Menu_baseDestroy(&self->m_base);
}

bool FactoryReset_init(FactoryReset *self)
{
  Menu *menu = &self->m_base;
  if (!Menu_baseInit(menu)) {
    return false;
  }
  // bypass led selection for fac reset if a multi was set on
  // the current slot because it doesn't make sense to pick
  Mode *cur = Modes_curMode();
  if (!cur) {
    return false;
  }
  if (Mode_isMultiLed(cur)) {
    menu->m_ledSelected = true;
    menu->m_targetLeds = MAP_LED(LED_MULTI);
  }
  if (!menu->m_advanced) {
    // skip led selection
    menu->m_ledSelected = true;
  }
  // Start on exit by default
  menu->m_curSelection = false;
  DEBUG_LOG("Entered factory reset");
  return true;
}

MenuAction FactoryReset_run(FactoryReset *self)
{
  Menu *menu = &self->m_base;
  MenuAction result = Menu_runBase(menu);
  if (result != MENU_CONTINUE) {
    return result;
  }
  FactoryReset_showReset(self);
  return MENU_CONTINUE;
}

void FactoryReset_onShortClick(FactoryReset *self)
{
  Menu *menu = &self->m_base;
  menu->m_curSelection = (uint8_t)!menu->m_curSelection;
}

void FactoryReset_onLongClick(FactoryReset *self)
{
  Menu *menu = &self->m_base;
  if (menu->m_curSelection == 0) {
    // if the selection isn't actually on factory reset then just leave
    Menu_leaveMenu(menu, false);
    return;
  }
  // if the button hasn't been held long enough just return
  if (Button_holdDuration(g_pButton) <= (FACTORY_RESET_THRESHOLD_TICKS + MS_TO_TICKS(10))) {
    return;
  }
  // the button was held down long enough so actually perform the factory reset
  // restore defaults and then leave menu and save
  if (menu->m_advanced) {
    // reset the target mode slot on the target led
    const DefaultModeEntry *defMode = &defaultModes[Modes_curModeIndex()];
    Mode *cur = Modes_curMode();
    MAP_FOREACH_LED(menu->m_targetLeds) {
      const DefaultLedEntry *led = &defMode->leds[pos];
      Colorset set;
      Colorset_makeCols(&set, led->numColors, led->cols);
      Mode_setPattern(cur, led->patternID, pos, NULL, &set);
    }
    Mode_initPatterns(cur);
  } else {
    Leds_setBrightness(DEFAULT_BRIGHTNESS);
    VortexEngine_setAutoCycle(false);
    Modes_setDefaults();
    Modes_setCurMode(0);
    Modes_resetFlags();
  }
  Menu_leaveMenu(menu, true);
}

void FactoryReset_showReset(FactoryReset *self)
{
  Menu *menu = &self->m_base;
  if (menu->m_curSelection == 0) {
    Leds_clearAll();
    Leds_blinkAll(350, 350, RGB_WHITE0);
    return;
  }
  bool isPressed = Button_isPressed(g_pButton);
  if (!isPressed) {
    Leds_clearAll();
    Leds_blinkAll(50, 50, RGB_RED4);
    return;
  }
  // don't start the fill until the button has been held for a bit
  uint32_t holdDur = Button_holdDuration(g_pButton);
  if (holdDur < MS_TO_TICKS(100)) {
    return;
  }
  uint16_t progress = ((holdDur * 100) / FACTORY_RESET_THRESHOLD_TICKS);
  DEBUG_LOGF("progress: %d", progress);
  if (progress >= 100) {
    Leds_setAll(RGB_WHITE);
    return;
  }
  uint8_t offMs = 100;
  uint8_t onMs = (progress > 60) ? 30 : 100;
  uint8_t sat = (uint8_t)((progress * 5) >> 1); // Using bit shift for division by 2
  HSVColor hsv = HSVColor_makeHSV(0, 255 - sat, 180);
  Leds_clearAll();
  Leds_blinkIndex(LED_0, offMs, onMs, RGBColor_fromHSV(&hsv));
  Leds_blinkIndex(LED_1, offMs, onMs, RGB_WHITE0);
}

static void FactoryReset_onShortClickHook(void *self)
{
  FactoryReset_onShortClick((FactoryReset *)self);
}

static void FactoryReset_onLongClickHook(void *self)
{
  FactoryReset_onLongClick((FactoryReset *)self);
}

static bool FactoryReset_initHook(void *self)
{
  return FactoryReset_init((FactoryReset *)self);
}

static MenuAction FactoryReset_runHook(void *self)
{
  return FactoryReset_run((FactoryReset *)self);
}

static void FactoryReset_destroyHook(void *self)
{
  FactoryReset_destroy((FactoryReset *)self);
}
