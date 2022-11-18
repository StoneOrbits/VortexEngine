#include "GlobalBrightness.h"

#include "../../Modes/Modes.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

GlobalBrightness::GlobalBrightness() :
  Menu()
{
}

bool GlobalBrightness::init()
{
  if (!Menu::init()) {
    return false;
  }
  // would be nice if there was a more elegant way to do this
  for (uint32_t i = 0; i < 4; ++i) {
    if (m_brightnessOptions[i] == Leds::getBrightness()) {
      // make sure the default selection matches cur value
      m_curSelection = (Finger)i;
    }
  }
  DEBUG_LOG("Entered global brightness");
  return true;
}

bool GlobalBrightness::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  // display brightnesses on each finger
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    Leds::setFinger(f, HSVColor(0, 0, m_brightnessOptions[f]));
  }

  // blink the current selection
  blinkSelection();

  // continue
  return true;
}

void GlobalBrightness::onShortClick()
{
  // four options in global brightness
  m_curSelection = (Finger)(((uint32_t)m_curSelection + 1) % (FINGER_THUMB + 1));
}

void GlobalBrightness::onLongClick()
{
  // set the global brightness
  Leds::setBrightness(m_brightnessOptions[m_curSelection]);
  // done here
  leaveMenu();
}
