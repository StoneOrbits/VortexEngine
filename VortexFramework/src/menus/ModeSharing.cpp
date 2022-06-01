#include "ModeSharing.h"

#include "../TimeControl.h"
#include "../Infrared.h"
#include "../Leds.h"
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
  return true;
}

bool ModeSharing::run()
{
  if (!Menu::run()) {
    return false;
  }
  switch (m_sharingMode) {
  case SharingMode::SHARE_SEND:
    sendMode();
    break;
  case SharingMode::SHARE_RECEIVE:
    receiveMode();
    break;
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

void ModeSharing::sendMode()
{
  Leds::setRange(LED_FIRST, LED_LAST, RGB_WHITE);
  Infrared::write();
  m_sharingMode = SharingMode::SHARE_RECEIVE;
}

void ModeSharing::receiveMode()
{
  RGBColor col = RGB_TEAL;
  if (Infrared::read()) {
    col = RGB_PURPLE;
  }
  Leds::clearAll();
  LedPos pos = (LedPos)((Time::getCurtime() / Time::msToTicks(100)) % LED_COUNT);
  Leds::setRange(pos, LED_LAST, col);
}
