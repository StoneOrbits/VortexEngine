#include "FactoryReset.h"

#include "../../Log/Log.h"

FactoryReset::FactoryReset() :
  Menu()
{
}

FactoryReset::~FactoryReset()
{
}

bool FactoryReset::init()
{
  if (!Menu::init()) {
    return false;
  }
  DEBUG_LOG("Entered factory reset");
  return true;
}

bool FactoryReset::run()
{
  if (!Menu::run()) {
    return false;
  }
  return true;
}
