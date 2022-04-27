#include "ModeSharing.h"

#include "../Log.h"

ModeSharing::ModeSharing() :
  Menu()
{
}

bool ModeSharing::init(Mode *curMode)
{
  if (!Menu::init(curMode)) {
    return false;
  }

  return true;
}

bool ModeSharing::run()
{
  if (!Menu::run()) {
    return false;
  }
  return true;
}

// handlers for clicks
void ModeSharing::onShortClick()
{
}

void ModeSharing::onLongClick()
{
  leaveMenu();
}
