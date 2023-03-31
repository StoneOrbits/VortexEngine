#include "Randomizer.h"

#include "../../Log/Log.h"

Randomizer::Randomizer() :
  Menu()
{
}

Randomizer::~Randomizer()
{
}

bool Randomizer::init()
{
  if (!Menu::init()) {
    return false;
  }
  DEBUG_LOG("Entered randomizer");
  return true;
}

bool Randomizer::run()
{
  if (!Menu::run()) {
    return false;
  }
  return true;
}

void Randomizer::onShortClick()
{
}

void Randomizer::onLongClick()
{
}
