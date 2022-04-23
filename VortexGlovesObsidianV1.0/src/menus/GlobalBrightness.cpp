#include "GlobalBrightness.h"

#include "../LedControl.h"

GlobalBrightness::GlobalBrightness() :
  Menu()
{
}

bool GlobalBrightness::init(Mode *curMode)
{
  if (!Menu::init(curMode)) {
    return false;
  }
  // TODO: get global brightness and preset selection
  // m_curSelection = ?
  return true;
}

bool GlobalBrightness::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  // handle base menu logic
  if (!Menu::run(timeControl, button, ledControl)) {
    return false;
  }

  // display brightnesses on each finger
  ledControl->setFinger(FINGER_PINKIE, HSVColor(0, 0, 50));
  ledControl->setFinger(FINGER_RING, HSVColor(0, 0, 120));
  ledControl->setFinger(FINGER_MIDDLE, HSVColor(0, 0, 185));
  ledControl->setFinger(FINGER_INDEX, HSVColor(0, 0, 255));

  // blink the current selection
  blinkSelection(timeControl, ledControl);

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
  // done in the pattern select menu
  leaveMenu();
}
