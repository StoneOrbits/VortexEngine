#include "GlobalBrightness.h"

#include "../LedControl.h"
#include "../Settings.h"

#include "../Log.h"

GlobalBrightness::GlobalBrightness() :
  Menu()
{
}

bool GlobalBrightness::init(Mode *curMode)
{
  if (!Menu::init(curMode)) {
    return false;
  }
  // would be nice if there was a more elegant way to do this
  for (uint32_t i = 0; i < 4; ++i) {
    if (m_brightnessOptions[i] == g_pSettings->getBrightness()) {
      // make sure the default selection matches cur value
      m_curSelection = (Finger)i;
    }
  }

  DEBUG("Entered global brightness");

  return true;
}

bool GlobalBrightness::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  // display brightnesses on each finger
  for (Finger finger = FINGER_PINKIE; finger <= FINGER_INDEX; ++finger) {
    g_pLedControl->setFinger(finger, HSVColor(0, 0, m_brightnessOptions[finger]));
  }

  // blink the current selection
  blinkSelection();

  // continue
  return true;
}

void GlobalBrightness::onShortClick()
{
  // four options in global brightness
  m_curSelection = (Finger)(((uint32_t)m_curSelection + 1) % 4);
}

void GlobalBrightness::onLongClick()
{
  // set the global brightness
  g_pSettings->setBrightness(m_brightnessOptions[m_curSelection]); 
  // done here
  leaveMenu();
}
