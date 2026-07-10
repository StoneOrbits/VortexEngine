#include "GlobalBrightness.h"
#include <stdlib.h>

#include "../../VortexEngine.h"

#include "../../Patterns/PatternArgs.h"
#include "../../Patterns/Pattern.h"
#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Buttons/Button.h"
#include "../../Time/Timings.h"
#include "../../Menus/Menus.h"
#include "../../Modes/Modes.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#define COL32(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })

static const uint8_t s_brightnessOptions[4] = {
  BRIGHTNESS_OPTION_1, BRIGHTNESS_OPTION_2, BRIGHTNESS_OPTION_3, BRIGHTNESS_OPTION_4
};

#define NUM_BRIGHTNESS_OPTIONS (sizeof(s_brightnessOptions) / sizeof(s_brightnessOptions[0]))

static void showBrightnessSelection(GlobalBrightnessMenu *self);
static void setKeychainModeState(GlobalBrightnessMenu *self, KeychainModeState newState);
static MenuAction runKeychainMode(GlobalBrightnessMenu *self);

extern void VortexEngine_toggleForceSleep(bool enabled);
extern void VortexEngine_enterSleep(bool save);

Menu *GlobalBrightnessMenu_Create(RGBColor col, bool advanced)
{
  GlobalBrightnessMenu *self = (GlobalBrightnessMenu *)malloc(sizeof(GlobalBrightnessMenu));
  if (!self) return NULL;
  Menu_construct(&self->base, col, advanced);
  self->base.vtable = &g_globalBrightnessMenuVTable;
  self->keychainModeState = KEYCHAIN_MODE_STATE_OFF;
  self->lastStateChange = 0;
  self->colorIndex = 0;
  return &self->base;
}

void GlobalBrightnessMenu_destroy(Menu *self)
{
  (void)self;
  VortexEngine_toggleForceSleep(true);
}

bool GlobalBrightnessMenu_init(Menu *self)
{
  if (!Menu_init(self)) {
    return false;
  }
  GlobalBrightnessMenu *gb = (GlobalBrightnessMenu *)self;
  self->ledSelected = true;
  for (uint8_t i = 0; i < NUM_BRIGHTNESS_OPTIONS; ++i) {
    if (s_brightnessOptions[i] == Leds_getBrightness()) {
      self->curSelection = i;
    }
  }
  if (self->advanced) {
    VortexEngine_toggleForceSleep(false);
    Modes_setKeychainMode(true, true);
    setKeychainModeState(gb, KEYCHAIN_MODE_STATE_SOLID);
    gb->colorIndex = 0;
  }
  DEBUG_LOG("Entered global brightness");
  return true;
}

MenuAction GlobalBrightnessMenu_run(Menu *self)
{
  MenuAction result = Menu_run(self);
  if (result != MENU_CONTINUE) {
    return result;
  }
  GlobalBrightnessMenu *gb = (GlobalBrightnessMenu *)self;
  if (self->advanced) {
    return runKeychainMode(gb);
  }
  showBrightnessSelection(gb);
  Menus_showSelection(COL32(RGB_WHITE5));
  return MENU_CONTINUE;
}

void GlobalBrightnessMenu_onShortClick(Menu *self)
{
  if (self->advanced) {
    return;
  }
  self->curSelection = (self->curSelection + 1) % (NUM_BRIGHTNESS_OPTIONS + 1);
}

void GlobalBrightnessMenu_onLongClick(Menu *self)
{
  if (self->advanced) {
    return;
  }
  if (self->curSelection >= NUM_BRIGHTNESS_OPTIONS) {
    Menu_leaveMenu(self, false);
    return;
  }
  Leds_setBrightness(s_brightnessOptions[self->curSelection]);
  Menu_leaveMenu(self, true);
}

static void showBrightnessSelection(GlobalBrightnessMenu *self)
{
  if (self->base.curSelection >= NUM_BRIGHTNESS_OPTIONS) {
    Menu_showExit(&self->base);
    return;
  }
  HSVColor hsv;
  HSVColor_initHSV(&hsv, 38, 255, s_brightnessOptions[self->base.curSelection]);
  RGBColor rgb;
  RGBColor_initFromHSV(&rgb, &hsv);
  Leds_setAll(rgb);
}

// ==================== KEYCHAIN_MODE STUFF ====================

#define KEYCHAIN_MODE_TIMER_MS      2100
#define KEYCHAIN_MODE_TIMER_TICKS   MS_TO_TICKS(KEYCHAIN_MODE_TIMER_MS)
#define KEYCHAIN_MODE_EXIT_CLICKS   8
#define KEYCHAIN_MODE_SLEEP_TICKS   SEC_TO_TICKS(300)

static MenuAction runKeychainMode(GlobalBrightnessMenu *self)
{
  if (Button_onConsecutivePresses(g_pButton, KEYCHAIN_MODE_EXIT_CLICKS)) {
    Modes_setKeychainMode(false, true);
    return MENU_QUIT;
  }
  uint32_t now = Time_getCurtime();
  if (Button_onPress(g_pButton) && self->keychainModeState != KEYCHAIN_MODE_STATE_OFF && now > (self->lastStateChange + KEYCHAIN_MODE_TIMER_TICKS)) {
    setKeychainModeState(self, KEYCHAIN_MODE_STATE_OFF);
    return MENU_CONTINUE;
  }
  if (Button_onRelease(g_pButton) && Button_pressTime(g_pButton) > self->lastStateChange) {
    setKeychainModeState(self, (KeychainModeState)(self->keychainModeState + 1));
    return MENU_CONTINUE;
  }
  if (Button_isPressed(g_pButton)) {
    Leds_clearAll();
    return MENU_CONTINUE;
  }
  if (self->keychainModeState == KEYCHAIN_MODE_STATE_OFF && now > (self->lastStateChange + KEYCHAIN_MODE_SLEEP_TICKS)) {
    VortexEngine_enterSleep(false);
    return MENU_QUIT;
  }
  if (self->keychainModeState == KEYCHAIN_MODE_STATE_SIGNAL) {
    Leds_clearAll();
    Colorset cs = Mode_getColorset(&self->base.previewMode, LED_ALL);
    RGBColor col = Colorset_get(&cs, self->colorIndex);
    Leds_blinkIndexOffset(LED_ALL,
      Time_getCurtime() - self->lastStateChange,
      SIGNAL_OFF_DURATION,
      SIGNAL_ON_DURATION,
      col);
  } else {
    Mode_play(&self->base.previewMode);
  }
  return MENU_CONTINUE;
}

static void setKeychainModeState(GlobalBrightnessMenu *self, KeychainModeState newState)
{
  self->keychainModeState = (KeychainModeState)(newState % KEYCHAIN_MODE_STATE_COUNT);
  self->lastStateChange = Time_getCurtime();
  Colorset curColorset = Mode_getColorset(Modes_curMode(), LED_ALL);
  uint8_t numCols = Colorset_numColors(&curColorset);
  PatternArgs args;
  PatternArgs_init(&args);
  switch (self->keychainModeState) {
  case KEYCHAIN_MODE_STATE_OFF:
  default:
    self->colorIndex++;
    if (self->colorIndex >= numCols) {
      self->colorIndex = 0;
    }
    break;
  case KEYCHAIN_MODE_STATE_SOLID:
    PatternArgs_init1(&args, 200);
    break;
  case KEYCHAIN_MODE_STATE_DOPS:
    PatternArgs_init2(&args, 1, 10);
    break;
  case KEYCHAIN_MODE_STATE_SIGNAL:
    break;
  }
  args.arg6 = self->colorIndex;
  Mode_setPattern(&self->base.previewMode, PATTERN_SOLID, LED_ALL, &args, NULL);
  Mode_init(&self->base.previewMode);
}

const MenuVTable g_globalBrightnessMenuVTable = {
  .destroy = GlobalBrightnessMenu_destroy,
  .init = GlobalBrightnessMenu_init,
  .run = GlobalBrightnessMenu_run,
  .onLedSelected = Menu_onLedSelected,
  .onShortClick = GlobalBrightnessMenu_onShortClick,
  .onLongClick = GlobalBrightnessMenu_onLongClick,
  .leaveMenu = Menu_leaveMenu,
};
