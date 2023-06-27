#include "GlobalBrightness.h"

#include "../../Patterns/PatternArgs.h"
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
  m_lastStateChange(0)
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
#define INOVA_TIMER_MS    2100
#define INOVA_TIMER_TICKS Time::msToTicks(INOVA_TIMER_MS)
#define INOVA_EXIT_HOLD   2000

// bonus simulate inova in this menu
Menu::MenuAction GlobalBrightness::runInova()
{
  uint32_t nextAlarm = (m_lastStateChange + INOVA_TIMER_TICKS);
  bool pressedBefore = (g_pButton->pressTime() < nextAlarm);
  // when the button is pressed
  if (g_pButton->isPressed()) {
    if (m_inovaState == INOVA_STATE_OFF && g_pButton->pressTime() > m_lastStateChange) {
      setInovaState(INOVA_STATE_SOLID);
    } else if (pressedBefore) {
      Leds::clearAll();
    } else {
      setInovaState(INOVA_STATE_OFF);
    }
    return MENU_CONTINUE;
  }
  // when the button is released
  if (g_pButton->onRelease()) {
    // detect quit
    if (g_pButton->holdDuration() > INOVA_EXIT_HOLD && m_inovaState == INOVA_STATE_SOLID) {
      // don't save, nothing changed
      leaveMenu(false);
    }
    if ((m_inovaState > INOVA_STATE_SOLID || g_pButton->pressTime() > m_lastStateChange)) {
      // advance the state or turn off based on press time
      setInovaState(pressedBefore ? (inova_state)(m_inovaState + 1) : INOVA_STATE_OFF);
    }
  }
  // check if the timer has run out
  if (m_inovaState != INOVA_STATE_OFF && m_inovaState != INOVA_STATE_SIGNAL && nextAlarm < Time::getCurtime()) {
    // inova alarm triggered, switch off
    setInovaState(INOVA_STATE_OFF);
  }
  if (m_inovaState == INOVA_STATE_OFF) {
    Leds::clearAll();
  } else {
    // play the inova mode
    m_inovaMode.play();
  }
  return MENU_CONTINUE;
}

void GlobalBrightness::setInovaState(inova_state newState)
{
  // update the inova state
  m_inovaState = (inova_state)(newState % INOVA_STATE_COUNT);
  // record the time of this statechange
  m_lastStateChange = Time::getCurtime();
  PatternID newID = PATTERN_DOPS;
  PatternArgs args;
  switch (m_inovaState) {
  case INOVA_STATE_OFF:
  default:
    Leds::clearAll();
    // just return
    return;
  case INOVA_STATE_SOLID:
    // just 0 offtime
    args.arg1 = 200;
    args.arg2 = 0;
    break;
  case INOVA_STATE_DOPS:
    // args for dops
    args.arg1 = DOPS_ON_DURATION;
    args.arg2 = DOPS_OFF_DURATION;
    break;
  case INOVA_STATE_SIGNAL:
    // signal blink timing
    args.arg1 = 16;
    args.arg2 = 120;
    break;
  }
  // update mode and ensure current colorset is always used
  m_inovaMode.setPattern(newID, LED_ALL, &args);
  m_inovaMode.setColorset(m_pCurMode->getColorset(), LED_ALL);
  m_inovaMode.init();
}
