#include "ModeSharing.h"

#include "../Log.h"

ModeSharing::ModeSharing() :
  Menu(),
  m_sharingMode(SharingMode::SHARE_SEND)
{
}

bool ModeSharing::init()
{
  if (!Menu::init()) {
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
  if (m_sharingMode == SharingMode::SHARE_SEND) {
    m_sharingMode = SharingMode::SHARE_RECEIVE;
  } else {
    m_sharingMode = SharingMode::SHARE_RECEIVE;
  }
}

void ModeSharing::onLongClick()
{
  leaveMenu();
}
