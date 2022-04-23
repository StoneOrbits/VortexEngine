#include "GlobalBrightness.h"

GlobalBrightness::GlobalBrightness() :
  Menu()
{
}

bool GlobalBrightness::init(Mode *curMode)
{
  if (!Menu::init(curMode)) {
    return false;
  }
  return true;
}

bool GlobalBrightness::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  // handle base menu logic
  if (!Menu::run(timeControl, button, ledControl)) {
    return false;
  }

  // TODO: display brightnesses


  // continue
  return true;
}

void GlobalBrightness::onShortClick()
{
  // four options in global brightness
  m_curSelection = (m_curSelection + 1) % 4;
}

void GlobalBrightness::onLongClick()
{
  // done in the pattern select menu
  leaveMenu();
}
