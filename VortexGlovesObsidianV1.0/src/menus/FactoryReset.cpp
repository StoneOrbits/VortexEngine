#include "FactoryReset.h"

FactoryReset::FactoryReset() :
  Menu()
{
}

bool FactoryReset::init(Mode *curMode)
{
  if (!Menu::init(curMode)) {
    return false;
  }
  return true;
}

bool FactoryReset::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  // handle base menu logic
  if (!Menu::run(timeControl, button, ledControl)) {
    return false;
  }

  // TODO: display factory reset options

  // continue
  return true;
}

void FactoryReset::onShortClick()
{
}

void FactoryReset::onLongClick()
{
  // done in the pattern select menu
  leaveMenu();
}
