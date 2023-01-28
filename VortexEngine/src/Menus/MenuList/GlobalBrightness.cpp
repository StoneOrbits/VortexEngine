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
      m_curSelection = (Quadrant)i;
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
  for (Quadrant f = QUADRANT_FIRST; f <= QUADRANT_4; ++f) {
    Leds::setQuadrant(f, HSVColor(0, 0, m_brightnessOptions[f]));
  }

  // blink the current selection
  blinkSelection();

  // continue
  return true;
}

void GlobalBrightness::onShortClick()
{
  // four options in global brightness
  m_curSelection = (Quadrant)(((uint32_t)m_curSelection + 1) % (QUADRANT_COUNT));
}

void GlobalBrightness::onShortClick2()
{
  // four options in global brightness
  if (!m_curSelection) {
    m_curSelection = QUADRANT_LAST;
  } else {
    m_curSelection = m_curSelection - 1;
  }
}

void GlobalBrightness::onLongClick()
{
  if (m_curSelection == QUADRANT_LAST) {
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

void GlobalBrightness::onLongClick2()
{
  leaveMenu();
}
