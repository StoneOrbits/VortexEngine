#include "ColorSelect.h"

#include "../../Log/Log.h"

ColorSelect::ColorSelect() :
  Menu()
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

bool ColorSelect::run()
{
  if (!Menu::run()) {
    return false;
  }
  return true;
}
