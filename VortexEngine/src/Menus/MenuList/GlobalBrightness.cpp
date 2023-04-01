#include "GlobalBrightness.h"

#include "../../Log/Log.h"

GlobalBrightness::GlobalBrightness() :
  Menu()
{
}

GlobalBrightness::~GlobalBrightness()
{
}

bool GlobalBrightness::init()
{
  if (!Menu::init()) {
    return false;
  }
  DEBUG_LOG("Entered global brightness");
  return true;
}

bool GlobalBrightness::run()
{
  if (!Menu::run()) {
    return false;
  }
  return true;
}
