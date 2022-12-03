#include "FactoryReset.h"

#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Modes/Modes.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

FactoryReset::FactoryReset() :
  Menu()
{
}

bool FactoryReset::init()
{
  if (!Menu::init()) {
    return false;
  }
  // start on exit by default
  m_curSelection = FINGER_THUMB;
  DEBUG_LOG("Entered factory reset");
  return true;
}

bool FactoryReset::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  // show the reset menu
  showReset();

  // blink the selection
  blinkSelection();

  // continue
  return true;
}

void FactoryReset::onShortClick()
{
  if (m_curSelection == FINGER_THUMB) {
    m_curSelection = FINGER_COUNT;
  } else {
    m_curSelection = FINGER_THUMB;
  }
}

void FactoryReset::onLongClick()
{
  switch (m_curSelection) {
  case FINGER_THUMB:
    leaveMenu();
    return;
  case FINGER_COUNT:
    if (g_pButton->holdDuration() > FACTORY_RESET_THRESHOLD_TICKS) {
      Modes::setDefaults();
      leaveMenu(true);
    }
    break;
  }
}

void FactoryReset::showReset()
{
  Leds::clearRange(LED_FIRST, INDEX_TOP);
  if (m_curSelection != FINGER_COUNT) {
    return;
  }
  if (g_pButton->isPressed()) {
    uint32_t duration = (g_pButton->holdDuration() * INDEX_TOP) / FACTORY_RESET_THRESHOLD_TICKS;
    if (duration <= INDEX_TOP) {
      Leds::blinkRange(LED_FIRST, (LedPos)(INDEX_TOP - duration), Time::getCurtime(), 
        150 - (12 * duration), 200 - (10 * duration), HSVColor(HUE_YELLOW - (10 * duration), 255, 255));
    } else {
      Leds::blinkRange(LED_FIRST, INDEX_TOP, Time::getCurtime(), 80, 60, RGB_WHITE);
    }
  } else {
    Leds::blinkRange(LED_FIRST, INDEX_TOP, Time::getCurtime(), 250, 150, RGB_BLANK);
  }
}
