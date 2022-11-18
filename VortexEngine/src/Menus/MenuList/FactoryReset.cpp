#include "FactoryReset.h"

#include "../../Time/TimeControl.h"
#include "../../Modes/Modes.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

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
  DEBUG_LOG("Entered factory reset");
  return true;
}

bool FactoryReset::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  // set all to dim red, or brighter if confirming
  Leds::setAll(HSVColor(HUE_RED, 255, 50 + 100 * m_confirm));
  Leds::blinkAll(m_confirm ? 100 : 500);

  // continue
  return true;
}

void FactoryReset::onShortClick()
{
  m_confirm = !m_confirm;
  DEBUG_LOGF("Factory reset confirm = %s", m_confirm ? "Yes" : "No");
}

void FactoryReset::onLongClick()
{
  if (m_confirm) {
    // perform the actual reset to default
    Modes::setDefaults();
    DEBUG_LOG("Restoring factory settings");
  } else {
    DEBUG_LOG("Exiting factory reset");
  }
  // done here
  leaveMenu();
}
