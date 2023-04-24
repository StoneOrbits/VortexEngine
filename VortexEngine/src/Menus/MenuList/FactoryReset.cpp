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
#include <Arduino.h>

FactoryReset::FactoryReset(const RGBColor &col) :
  Menu(col)
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
  uint8_t curModeIndex = Modes::curModeIndex();
  // reset the target mode slot on the target led
  const default_mode_entry &def = default_modes[curModeIndex];
  Colorset set(def.numColors, def.cols);
  m_pCurMode->setPatternMap(m_targetLeds, def.patternID, nullptr, &set);
  // re-initialize the current mode
  m_pCurMode->init();
  // save and leave the menu
  leaveMenu(true);
}

void FactoryReset::showReset()
{
  if (m_curSelection == 0) {
    Leds::clearAll();
    Leds::blinkAll(Time::getCurtime(), 350, 350, RGB_BLANK);
    return;
  }

  if (!g_pButton->isPressed()) {
    Leds::clearAll();
    Leds::blinkAll(Time::getCurtime(), 150, 150, RGB_DIM_RED);
    return;
  }

  // don't start the fill until the button has been held for a bit
  uint32_t holdDur = g_pButton->holdDuration();
  if (holdDur < Time::msToTicks(100)) {
    return;
  }

  uint16_t progress = ((holdDur * 100) / FACTORY_RESET_THRESHOLD_TICKS);

  DEBUG_LOGF("progress: %f", progress);

  if (progress >= 100) {
    Leds::setAll(RGB_WHITE);
    return;
  }

  uint32_t offMs = 120 - (progress / 2);
  uint32_t onMs = 120 - (progress / 2);
  int8_t sat = (int8_t)(2.5 * progress);

  Leds::clearAll();
  Leds::blinkAll(Time::getCurtime(), offMs, onMs, HSVColor(0, 255 - sat, 180));
}
