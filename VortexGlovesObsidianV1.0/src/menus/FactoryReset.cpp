#include "FactoryReset.h"

FactoryReset::FactoryReset() :
  Menu(HSV_RED)
{
}

bool FactoryReset::run(const Button *button, LedControl *ledControl)
{
  return true;
}
