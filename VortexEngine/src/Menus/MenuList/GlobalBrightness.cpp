#include "GlobalBrightness.h"

#include "../../Log/Log.h"

GlobalBrightness::GlobalBrightness(const RGBColor &col) :
  Menu(col)
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

Menu::MenuAction GlobalBrightness::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }
  return MENU_CONTINUE;
}
