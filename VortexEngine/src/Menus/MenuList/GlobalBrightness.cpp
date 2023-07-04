#include "GlobalBrightness.h"

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

#define NUM_BRIGHTNESS_OPTIONS (sizeof(m_brightnessOptions) / sizeof(m_brightnessOptions[0]))

GlobalBrightness::GlobalBrightness(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_keychain_modeState(KEYCHAIN_MODE_STATE_OFF),
  m_lastStateChange(0),
  m_colorIndex(0)
{
}

GlobalBrightness::~GlobalBrightness()
{
}

bool GlobalBrightness::init()
{
  if (!Menu::init()) {
    return false;
  }
  // bypass led selection
  m_ledSelected = true;
  // would be nice if there was a more elegant way to do this
  for (uint8_t i = 0; i < NUM_BRIGHTNESS_OPTIONS; ++i) {
    if (m_brightnessOptions[i] == Leds::getBrightness()) {
      // make sure the default selection matches cur value
      m_curSelection = i;
    }
  }
  if (m_advanced) {
    // start in the off position, this call is necessary to update the
    // lastStateChange time to the current time of init
    setKeychainModeState(KEYCHAIN_MODE_STATE_OFF);
    // reset the color index to ensure it starts on first color
    m_colorIndex = 0;
  }
  DEBUG_LOG("Entered global brightness");
  return true;
}

Menu::MenuAction GlobalBrightness::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }

  if (m_advanced) {
    return runKeychainMode();
  }

  // show the current brightness
  showBrightnessSelection();

  // show selections
  Menus::showSelection();

  // continue
  return MENU_CONTINUE;
}

void GlobalBrightness::onShortClick()
{
  if (m_advanced) {
    return;
  }
  // include one extra option for the exit slot
  m_curSelection = (m_curSelection + 1) % (NUM_BRIGHTNESS_OPTIONS + 1);
}

void GlobalBrightness::onLongClick()
{
  if (m_advanced) {
    return;
  }
  if (m_curSelection >= NUM_BRIGHTNESS_OPTIONS) {
    // no save exit
    leaveMenu();
    return;
  }
  // need to save if the new brightness is different
  bool needsSave = (Leds::getBrightness() != m_brightnessOptions[m_curSelection]);
  // set the global brightness
  Leds::setBrightness(m_brightnessOptions[m_curSelection]);
  // done here, save settings with new brightness
  leaveMenu(needsSave);
}

void GlobalBrightness::showBrightnessSelection()
{
  if (m_curSelection >= NUM_BRIGHTNESS_OPTIONS) {
    showExit();
    return;
  }
  Leds::setAll(HSVColor(38, 255, m_brightnessOptions[m_curSelection]));
}

// ==================== KEYCHAIN_MODE STUFF ====================

// don't worry about this stuff
#define KEYCHAIN_MODE_TIMER_MS      2100
#define KEYCHAIN_MODE_TIMER_TICKS   Time::msToTicks(KEYCHAIN_MODE_TIMER_MS)
#define KEYCHAIN_MODE_EXIT_CLICKS   8
#define KEYCHAIN_MODE_SLEEP_S       10
#define KEYCHAIN_MODE_SLEEP_TICKS   Time::secToTicks(KEYCHAIN_MODE_SLEEP_S)

// bonus simulate keychain_mode in this menu
Menu::MenuAction GlobalBrightness::runKeychainMode()
{
  // check for exit
  if (g_pButton->consecutivePresses() > KEYCHAIN_MODE_EXIT_CLICKS) {
    return MENU_QUIT;
  }
  uint32_t now = Time::getCurtime();
  // whether the button was pressed before the timer expired
  // when the button is first pressed after the window has expired switch off
  if (g_pButton->onPress() && m_keychain_modeState != KEYCHAIN_MODE_STATE_OFF && now > (m_lastStateChange + KEYCHAIN_MODE_TIMER_TICKS)) {
    setKeychainModeState(KEYCHAIN_MODE_STATE_OFF);
    return MENU_CONTINUE;
  }
  // when the button is released, but only if they pressed it within this state
  if (g_pButton->onRelease() && g_pButton->pressTime() > m_lastStateChange) {
    // advance the state or turn off based on press time
    setKeychainModeState((keychain_mode_state)(m_keychain_modeState + 1));
    return MENU_CONTINUE;
  }
  // as long as the button is held down the leds clear
  if (g_pButton->isPressed()) {
    Leds::clearAll();
    return MENU_CONTINUE;
  }
  // check for sleep after 10 seconds
  if (m_keychain_modeState == KEYCHAIN_MODE_STATE_OFF && now > (m_lastStateChange + KEYCHAIN_MODE_SLEEP_TICKS)) {
    VortexEngine::enterSleep();
    return MENU_QUIT;
  }
  // play the keychain_mode mode
  m_keychain_modeMode.play();
  return MENU_CONTINUE;
}

void GlobalBrightness::setKeychainModeState(keychain_mode_state newState)
{
  // update the keychain_mode state
  m_keychain_modeState = (keychain_mode_state)(newState % KEYCHAIN_MODE_STATE_COUNT);
  // record the time of this statechange
  m_lastStateChange = Time::getCurtime();
  // the args that will define the timing of the blink on the solid pattern
  PatternArgs args;
  switch (m_keychain_modeState) {
  case KEYCHAIN_MODE_STATE_OFF:
  default:
    // iterate to next color
    m_colorIndex = (m_colorIndex + 1) % m_pCurMode->getColorset().numColors();
    break;
  case KEYCHAIN_MODE_STATE_SOLID:
    args.init(200);
    break;
  case KEYCHAIN_MODE_STATE_DOPS:
    args.init(1, 10);
    break;
  case KEYCHAIN_MODE_STATE_SIGNAL:
    args.init(SIGNAL_ON_DURATION, SIGNAL_OFF_DURATION);
    break;
  }
  // update the mode and ensure current colorset is always used, use PATTERN_SOLID
  // because that will never iterate to the next color and allows us to force 
  // the color index via argument 6
  args.arg6 = m_colorIndex;
  m_keychain_modeMode.setPattern(PATTERN_SOLID, LED_ALL, &args);
  m_keychain_modeMode.setColorset(m_pCurMode->getColorset(), LED_ALL);
  m_keychain_modeMode.init();
}
