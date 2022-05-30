#include "FactoryReset.h"

#include "../TimeControl.h"
#include "../Modes.h"
#include "../Leds.h"
#include "../Log.h"

FactoryReset::FactoryReset() :
  Menu(),
  m_confirm(false)
{
}

bool FactoryReset::init()
{
  if (!Menu::init()) {
    return false;
  }
  // reset this just in case
  m_confirm = false;
  // factory reset blinks all lights
  m_curSelection = FINGER_COUNT;
  DEBUG("Entered factory reset");
  return true;
}

bool FactoryReset::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  // set all to dim red, or brighter if confirming
  Leds::setAll(HSVColor(HUE_RED, 255, 150 + 50 * m_confirm));

  // TODO: better blink
  // blink faster to indicate confirmation
  uint32_t blinkThreshold = m_confirm ? 5 : 2;
  if ((Time::getCurtime() % 10) > blinkThreshold) {
    Leds::clearAll();
  }

  // continue
  return true;
}

void FactoryReset::onShortClick()
{
  m_confirm = !m_confirm;
  DEBUGF("Factory reset confirm = %s", m_confirm ? "Yes" : "No");
}

void FactoryReset::onLongClick()
{
  if (m_confirm) {
    // perform the actual reset to default
    Modes::setDefaults();
    DEBUG("Restoring factory settings");
  } else {
    DEBUG("Exiting factory reset");
  }
  // done here
  leaveMenu();
}
