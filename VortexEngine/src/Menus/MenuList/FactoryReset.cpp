#include "FactoryReset.h"

#include "../../Time/TimeControl.h"
#include "../../Buttons/Button.h"
#include "../../Time/Timings.h"
#include "../../Modes/Modes.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

FactoryReset::FactoryReset() :
  Menu(),
  m_resetMode(false)
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
  m_resetMode = false;
  DEBUG_LOG("Entered factory reset");
  return true;
}

bool FactoryReset::run()
{
  if (!Menu::run()) {
    return false;
  }
  Leds::clearAll();
  if (m_resetMode) {
    // blink red with a darker red
    Leds::blinkIndex(LED_0, Time::getCurtime(), 250, 500, RGB_RED);
    Leds::blinkIndex(LED_1, Time::getCurtime(), 250, 500, RGBColor(0, 100, 0));
  } else {
    // blink grey with a dark green for exit
    Leds::blinkIndex(LED_0, Time::getCurtime(), 100, 200, RGBColor(40, 40, 40));
    Leds::blinkIndex(LED_1, Time::getCurtime(), 100, 200, RGBColor(60, 0, 0));
  }
  if (g_pButton->isPressed() && g_pButton->holdDuration() >= SHORT_CLICK_THRESHOLD_TICKS) {
    Leds::setAll(RGB_WHITE);
  }
  return true;
}

void FactoryReset::onShortClick()
{
  m_resetMode = !m_resetMode;
}

void FactoryReset::onLongClick()
{
  if (m_resetMode) {
    // do the reset
    Modes::clearModes();
    Modes::setDefaults();
    Modes::saveStorage();
  }
  leaveMenu(m_resetMode);
}
