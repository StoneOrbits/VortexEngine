#include "FactoryReset.h"

FactoryReset::FactoryReset() :
  Menu(HSV_RED)
{
}

bool FactoryReset::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  return true;
}
