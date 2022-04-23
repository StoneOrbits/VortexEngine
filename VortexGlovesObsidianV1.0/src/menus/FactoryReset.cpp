#include "FactoryReset.h"

#include "../TimeControl.h"
#include "../LedControl.h"

FactoryReset::FactoryReset() :
  Menu(),
  m_confirm(false)
{
}

bool FactoryReset::init(Mode *curMode)
{
  if (!Menu::init(curMode)) {
    return false;
  }
  // reset this just in case
  m_confirm = false;
  // factory reset blinks all lights
  m_curSelection = FINGER_COUNT;
  return true;
}

bool FactoryReset::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  // set all to dim red, or brighter if confirming
  g_pLedControl->setAll(HSVColor(HUE_RED, 255, 150 + 50 * m_confirm));

  // TODO: better blink
  // blink faster to indicate confirmation
  uint32_t blinkThreshold = m_confirm ? 100 : 500;
  if ((g_pTimeControl->getCurtime() % 1000) > blinkThreshold) {
    g_pLedControl->clearAll();
  }

  // continue
  return true;
}

void FactoryReset::onShortClick()
{
  m_confirm = !m_confirm;
}

void FactoryReset::onLongClick()
{
  if (m_confirm) {
    // TODO: the reset
  }
  // done here
  leaveMenu();
}
