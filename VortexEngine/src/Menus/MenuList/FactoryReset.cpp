#include "FactoryReset.h"

#include "../../Modes/DefaultModes.h"
#include "../../Time/TimeControl.h"
#include "../../Patterns/Pattern.h"
#include "../../Buttons/Button.h"
#include "../../Time/Timings.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include "../../VortexConfig.h"

FactoryReset::FactoryReset(const RGBColor &col, bool advanced) :
  Menu(col, advanced)
{
}

FactoryReset::~FactoryReset()
{
}

bool FactoryReset::init()
{
  if (!Menu::init()) {
    return false;
  }
  if (!m_advanced) {
    // skip led selection
    m_ledSelected = true;
  }
  // Start on exit by default
  m_curSelection = false;
  DEBUG_LOG("Entered factory reset");
  return true;
}

Menu::MenuAction FactoryReset::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }
  // bypass led selection for fac reset if a multi was set on
  // the current slot because it doesn't make sense to pick
  if (m_pCurMode->isMultiLed()) {
    m_ledSelected = true;
    m_targetLeds = MAP_LED(LED_MULTI);
  }
  showReset();
  return MENU_CONTINUE;
}

void FactoryReset::onShortClick()
{
  m_curSelection = (uint8_t)!m_curSelection;
}

void FactoryReset::onLongClick()
{
  if (m_curSelection == 0) {
    // if the selection isn't actually on factory reset then just leave
    leaveMenu();
    return;
  }
  // if the button hasn't been held long enough just return
  if (g_pButton->holdDuration() <= (FACTORY_RESET_THRESHOLD_TICKS + Time::msToTicks(10))) {
    return;
  }
  // the button was held down long enough so actually perform the factory reset
  // restore defaults and then leave menu and save
  if (m_advanced) {
    uint8_t curModeIndex = Modes::curModeIndex();
    // reset the target mode slot on the target led
    const default_mode_entry &def = default_modes[curModeIndex];
    Colorset set(def.numColors, def.cols);
    m_pCurMode->setPatternMap(m_targetLeds, def.patternID, nullptr, &set);
    // re-initialize the current mode
    m_pCurMode->init();
  } else {
    Modes::setDefaults();
  }
  leaveMenu(true);
}

void FactoryReset::showReset()
{
  uint64_t curTime = Time::getCurtime();
  if (m_curSelection == 0) {
    Leds::clearAll();
    Leds::blinkAll(curTime, 350, 350, RGB_BLANK);
    return;
  }
  bool isPressed = g_pButton->isPressed();
  if (!isPressed) {
    Leds::clearAll();
    Leds::blinkAll(curTime, 150, 150, RGB_DIM_RED);
    return;
  }
  // don't start the fill until the button has been held for a bit
  uint32_t holdDur = g_pButton->holdDuration();
  if (holdDur < Time::msToTicks(100)) {
    return;
  }
  uint16_t progress = ((holdDur * 100) / FACTORY_RESET_THRESHOLD_TICKS);
  DEBUG_LOGF("progress: %d", progress);
  if (progress >= 100) {
    Leds::setAll(RGB_WHITE);
    return;
  }
  uint8_t offMs = 120;
  uint8_t onMs = (progress > 60) ? 50 : 120; // Using bit shift for division by 2
  uint8_t sat = (uint8_t)((progress * 5) >> 1); // Using bit shift for division by 2
  Leds::clearAll();
  Leds::blinkIndex(LED_0, curTime, offMs, onMs, HSVColor(0, 255 - sat, 180));
  Leds::blinkIndex(LED_1, curTime, offMs, onMs, RGB_BLANK);
}

