#include "ColorSelect.h"

#include "../../Log/Log.h"

ColorSelect::ColorSelect(const RGBColor &col) :
  Menu(col)
{
}

ColorSelect::~ColorSelect()
{
}

bool ColorSelect::init()
{
  if (!Menu::init()) {
    return false;
  }
  DEBUG_LOG("Entered color select");
  return true;
}

Menu::MenuAction ColorSelect::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }
  return MENU_CONTINUE;
}
