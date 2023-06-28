#include "GlobalBrightness.h"

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
  m_inovaState(INOVA_STATE_OFF),
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
    return runInova();
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

// ==================== INOVA STUFF ====================

// don't worry about this stuff
#define INOVA_TIMER_MS      2100
#define INOVA_TIMER_TICKS   Time::msToTicks(INOVA_TIMER_MS)
#define INOVA_EXIT_CLICKS   8

// bonus simulate inova in this menu
Menu::MenuAction GlobalBrightness::runInova()
{
  // check for exit
  if (g_pButton->consecutivePresses() > INOVA_EXIT_CLICKS) {
    return MENU_QUIT;
  }
  // whether the button was pressed before the timer expired
  // when the button is first pressed after the window has expired switch off
  if (g_pButton->onPress() && m_inovaState != INOVA_STATE_OFF && Time::getCurtime() > (m_lastStateChange + INOVA_TIMER_TICKS)) {
    setInovaState(INOVA_STATE_OFF);
    return MENU_CONTINUE;
  }
  // when the button is released, but only if they pressed it within this state
  if (g_pButton->onRelease() && g_pButton->pressTime() > m_lastStateChange) {
    // advance the state or turn off based on press time
    setInovaState((inova_state)(m_inovaState + 1));
    return MENU_CONTINUE;
  }
  // as long as the button is held down the leds clear
  if (g_pButton->isPressed()) {
    Leds::clearAll();
    return MENU_CONTINUE;
  }
  // play the inova mode
  m_inovaMode.play();
  return MENU_CONTINUE;
}

void GlobalBrightness::setInovaState(inova_state newState)
{
  // update the inova state
  m_inovaState = (inova_state)(newState % INOVA_STATE_COUNT);
  // record the time of this statechange
  m_lastStateChange = Time::getCurtime();
  // the args that will define the timing of the blink on the solid pattern
  PatternArgs args;
  switch (m_inovaState) {
  case INOVA_STATE_OFF:
  default:
    // iterate to next color
    m_colorIndex = (m_colorIndex + 1) % m_pCurMode->getColorset().numColors();
    break;
  case INOVA_STATE_SOLID:
    args.init(200);
    break;
  case INOVA_STATE_DOPS:
    args.init(DOPS_ON_DURATION, DOPS_OFF_DURATION);
    break;
  case INOVA_STATE_SIGNAL:
    args.init(SIGNAL_ON_DURATION, SIGNAL_OFF_DURATION);
    break;
  }
  // update the mode and ensure current colorset is always used, use PATTERN_SOLID
  // because that will never iterate to the next color and allows us to force 
  // the color index via argument 6
  args.arg6 = m_colorIndex;
  m_inovaMode.setPattern(PATTERN_SOLID, LED_ALL, &args);
  m_inovaMode.setColorset(m_pCurMode->getColorset(), LED_ALL);
  m_inovaMode.init();
}
