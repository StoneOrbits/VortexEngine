#include "ModeSharing.h"

#include "../../Log/Log.h"

ModeSharing::ModeSharing() :
  Menu()
{
}

ModeSharing::~ModeSharing()
{
}

bool ModeSharing::init()
{
  if (!Menu::init()) {
    return false;
  }
  DEBUG_LOG("Entering Mode Sharing");
  return true;
}

bool ModeSharing::run()
{
  if (!Menu::run()) {
    return false;
  }
  return true;
}

void ModeSharing::onShortClick()
{
}

void ModeSharing::onLongClick()
{
}
