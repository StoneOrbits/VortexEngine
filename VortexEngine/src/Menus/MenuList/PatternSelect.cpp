#include "PatternSelect.h"

#include "../../Log/Log.h"

PatternSelect::PatternSelect() :
  Menu()
{
}

PatternSelect::~PatternSelect()
{
}

bool PatternSelect::init()
{
  if (!Menu::init()) {
    return false;
  }
  DEBUG_LOG("Entered pattern select");
  return true;
}

bool PatternSelect::run()
{
  if (!Menu::run()) {
    return false;
  }
  return true;
}

void PatternSelect::onShortClick()
{
}

void PatternSelect::onLongClick()
{
}
