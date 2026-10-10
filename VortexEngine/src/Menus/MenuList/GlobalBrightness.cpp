#include "GlobalBrightness.h"
#include "../Menus.h"

#include "../../VortexEngine.h"

#include "../../Patterns/PatternArgs.h"
#include "../../Patterns/Pattern.h"
#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Buttons/Button.h"
#include "../../Time/Timings.h"
#include "../../Menus/Menus.h"
#include "../../Modes/Modes.h"
#include "../../Time/Timer.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"

static void GlobalBrightness_destroyHook(void *self);
static bool GlobalBrightness_initHook(void *self);
static MenuAction GlobalBrightness_runHook(void *self);
static void GlobalBrightness_onShortClickHook(void *self);
static void GlobalBrightness_onLongClickHook(void *self);

const MenuVtable GlobalBrightness_vtable = {
  GlobalBrightness_initHook,
  GlobalBrightness_runHook,
  NULL,
  GlobalBrightness_onShortClickHook,
  GlobalBrightness_onLongClickHook,
  NULL,
  GlobalBrightness_destroyHook,
};

// the list of brightness options
static const uint8_t s_brightnessOptions[4] = {
  BRIGHTNESS_OPTION_1,
  BRIGHTNESS_OPTION_2,
  BRIGHTNESS_OPTION_3,
  BRIGHTNESS_OPTION_4
};

// allow the number of brightness options to be adjusted dynamically
#define NUM_BRIGHTNESS_OPTIONS (sizeof(s_brightnessOptions) / sizeof(s_brightnessOptions[0]))

GlobalBrightness *GlobalBrightness_new(const RGBColor *col, bool advanced)
{
  GlobalBrightness *self = (GlobalBrightness *)Menus_allocMenu();
  if (!self) {
    return NULL;
  }
  Menu_initBase(&self->m_base, &GlobalBrightness_vtable);
  RGBColor_setRGB(&self->m_base.m_menuColor, col);
  self->m_base.m_advanced = advanced;
  self->m_keychain_modeState = KEYCHAIN_MODE_STATE_OFF;
  self->m_lastStateChange = 0;
  self->m_colorIndex = 0;
  return self;
}

void GlobalBrightness_destroy(GlobalBrightness *self)
{
  // ensure force sleep is turned back on when we leave
  VortexEngine_toggleForceSleep(true);
  Menu_baseDestroy(&self->m_base);
}

bool GlobalBrightness_init(GlobalBrightness *self)
{
  Menu *menu = &self->m_base;
  if (!Menu_baseInit(menu)) {
    return false;
  }
  // bypass led selection
  menu->m_ledSelected = true;
  // would be nice if there was a more elegant way to do this
  for (uint8_t i = 0; i < NUM_BRIGHTNESS_OPTIONS; ++i) {
    if (s_brightnessOptions[i] == Leds_getBrightness()) {
      // make sure the default selection matches cur value
      menu->m_curSelection = i;
    }
  }
  if (menu->m_advanced) {
    // turn off force sleep while running keychain mode
    VortexEngine_toggleForceSleep(false);
    // enable keychain mode so if the device sleeps it will wake into keychain mode
    Modes_setKeychainMode(true, true);
    // start in the off position, this call is necessary to update the
    // lastStateChange time to the current time of init
    GlobalBrightness_setKeychainModeState(self, KEYCHAIN_MODE_STATE_SOLID);
    // reset the color index to ensure it starts on first color
    self->m_colorIndex = 0;
  }
  DEBUG_LOG("Entered global brightness");
  return true;
}

MenuAction GlobalBrightness_run(GlobalBrightness *self)
{
  Menu *menu = &self->m_base;
  MenuAction result = Menu_runBase(menu);
  if (result != MENU_CONTINUE) {
    return result;
  }
  if (menu->m_advanced) {
    return GlobalBrightness_runKeychainMode(self);
  }
  // show the current brightness
  GlobalBrightness_showBrightnessSelection(self);
  // show selections
  Menus_showSelection(RGB_WHITE5);
  // continue
  return MENU_CONTINUE;
}

void GlobalBrightness_onShortClick(GlobalBrightness *self)
{
  Menu *menu = &self->m_base;
  if (menu->m_advanced) {
    return;
  }
  // include one extra option for the exit slot
  menu->m_curSelection = (menu->m_curSelection + 1) % (NUM_BRIGHTNESS_OPTIONS + 1);
}

void GlobalBrightness_onLongClick(GlobalBrightness *self)
{
  Menu *menu = &self->m_base;
  if (menu->m_advanced) {
    return;
  }
  if (menu->m_curSelection >= NUM_BRIGHTNESS_OPTIONS) {
    // no save exit
    Menu_leaveMenu(menu, false);
    return;
  }
  // set the global brightness
  Leds_setBrightness(s_brightnessOptions[menu->m_curSelection]);
  // done here, save settings with new brightness
  Menu_leaveMenu(menu, true);
}

void GlobalBrightness_showBrightnessSelection(GlobalBrightness *self)
{
  Menu *menu = &self->m_base;
  if (menu->m_curSelection >= NUM_BRIGHTNESS_OPTIONS) {
    Menu_showExit(menu);
    return;
  }
  HSVColor hsv = HSVColor_makeHSV(38, 255, s_brightnessOptions[menu->m_curSelection]);
  Leds_setAll(RGBColor_fromHSV(&hsv));
}

// ==================== KEYCHAIN_MODE STUFF ====================

// The general timer for keychain mode to remain active
#define KEYCHAIN_MODE_TIMER_MS      2100
#define KEYCHAIN_MODE_TIMER_TICKS   MS_TO_TICKS(KEYCHAIN_MODE_TIMER_MS)
// the number of clicks needed to exit keychain mode
#define KEYCHAIN_MODE_EXIT_CLICKS   8
// This is the duration of idle time on blank before it will auto-sleep
// give a solid 300 seconds (5 minutes) before it goes to sleep
#define KEYCHAIN_MODE_SLEEP_TICKS   SEC_TO_TICKS(300)

// bonus simulate keychain_mode in this menu
MenuAction GlobalBrightness_runKeychainMode(GlobalBrightness *self)
{
  Menu *menu = &self->m_base;
  // check for exit
  if (Button_onConsecutivePresses(g_pButton, KEYCHAIN_MODE_EXIT_CLICKS)) {
    // turn off keychain mode now
    Modes_setKeychainMode(false, true);
    return MENU_QUIT;
  }
  uint32_t now = Time_getCurtime();
  // whether the button was pressed before the timer expired
  // when the button is first pressed after the window has expired switch off
  if (Button_onPress(g_pButton) && self->m_keychain_modeState != KEYCHAIN_MODE_STATE_OFF && now > (self->m_lastStateChange + KEYCHAIN_MODE_TIMER_TICKS)) {
    GlobalBrightness_setKeychainModeState(self, KEYCHAIN_MODE_STATE_OFF);
    return MENU_CONTINUE;
  }
  // when the button is released, but only if they pressed it within this state
  if (Button_onRelease(g_pButton) && Button_pressTime(g_pButton) > self->m_lastStateChange) {
    // advance the state or turn off based on press time
    GlobalBrightness_setKeychainModeState(self, (keychain_mode_state)(self->m_keychain_modeState + 1));
    return MENU_CONTINUE;
  }
  // as long as the button is held down the leds clear
  if (Button_isPressed(g_pButton)) {
    Leds_clearAll();
    return MENU_CONTINUE;
  }
  // check for sleep after some seconds
  if (self->m_keychain_modeState == KEYCHAIN_MODE_STATE_OFF && now > (self->m_lastStateChange + KEYCHAIN_MODE_SLEEP_TICKS)) {
    // Optional: indicate when the keychain is turning off (useful mainly for debugging)
    //Leds_holdAll(RGB_PURPLE);
    VortexEngine_enterSleep(false);
    return MENU_QUIT;
  }
  // render the signal mode in a different way
  if (self->m_keychain_modeState == KEYCHAIN_MODE_STATE_SIGNAL) {
    // use a custom blink because the SIGNAL_OFF duration is too large for uint8 params
    Colorset cs = Mode_getColorset(&menu->m_previewMode, LED_ANY);
    Leds_clearAll();
    Leds_blinkIndexOffset(LED_ALL,
      Time_getCurtime() - self->m_lastStateChange,
      SIGNAL_OFF_DURATION,
      SIGNAL_ON_DURATION,
      Colorset_get(&cs, self->m_colorIndex));
  } else {
    // play the keychain_mode mode
    Mode_play(&menu->m_previewMode);
  }
  return MENU_CONTINUE;
}

void GlobalBrightness_setKeychainModeState(GlobalBrightness *self, keychain_mode_state newState)
{
  Menu *menu = &self->m_base;
  // update the keychain_mode state
  self->m_keychain_modeState = (keychain_mode_state)(newState % KEYCHAIN_MODE_STATE_COUNT);
  // record the time of this statechange
  self->m_lastStateChange = Time_getCurtime();
  // this could
  // read the count from the pattern instead of copying the whole colorset
  const Pattern *pat = Mode_getPattern(Modes_curMode(), LED_ANY);
  uint8_t numCols = pat ? Colorset_numColors(&pat->m_colorset) : 0;
  // the args that will define the timing of the blink on the solid pattern
  PatternArgs args;
  PatternArgs_init0(&args);
  switch (self->m_keychain_modeState) {
  case KEYCHAIN_MODE_STATE_OFF:
  default:
    // iterate to next color and wrap to 0, don't use modulus because numCols could be 0
    self->m_colorIndex++;
    if (self->m_colorIndex >= numCols) {
      self->m_colorIndex = 0;
    }
    break;
  case KEYCHAIN_MODE_STATE_SOLID:
    PatternArgs_init(&args, 200);
    break;
  case KEYCHAIN_MODE_STATE_DOPS:
    PatternArgs_init(&args, 1, 10);
    break;
  case KEYCHAIN_MODE_STATE_SIGNAL:
    // unfortunately the signal blink timing is too large to fit into the uint8 params
    // so we just use a custom blink to render that one, but use the previewMode for
    // the other two states
    break;
  }
  // update the mode and ensure current colorset is always used, use PATTERN_SOLID
  // because that will never iterate to the next color and allows us to force
  // the color index via argument 6
  args.arg6 = self->m_colorIndex;
  Mode_setPattern(&menu->m_previewMode, PATTERN_SOLID, LED_ALL, &args, NULL);
  Mode_initPatterns(&menu->m_previewMode);
}

static void GlobalBrightness_onShortClickHook(void *self)
{
  GlobalBrightness_onShortClick((GlobalBrightness *)self);
}

static void GlobalBrightness_onLongClickHook(void *self)
{
  GlobalBrightness_onLongClick((GlobalBrightness *)self);
}

static bool GlobalBrightness_initHook(void *self)
{
  return GlobalBrightness_init((GlobalBrightness *)self);
}

static MenuAction GlobalBrightness_runHook(void *self)
{
  return GlobalBrightness_run((GlobalBrightness *)self);
}

static void GlobalBrightness_destroyHook(void *self)
{
  GlobalBrightness_destroy((GlobalBrightness *)self);
}
