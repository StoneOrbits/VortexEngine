#include "ModeSharing.h"

#include "../Infrared.h"
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
  DEBUG("Entering Mode Sharing");
  m_sharingMode = SharingMode::SHARE_SEND;
  switch (m_sharingMode) {
  case SharingMode::SHARE_SEND:
    Infrared::write();
    break;
  case SharingMode::SHARE_RECEIVE:
    Infrared::read();
    break;
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
    DEBUG("Switched to receive mode");
  } else {
    m_sharingMode = SharingMode::SHARE_SEND;
    DEBUG("Switched to send mode");
  }
}

void ModeSharing::onLongClick()
{
  leaveMenu();
}
