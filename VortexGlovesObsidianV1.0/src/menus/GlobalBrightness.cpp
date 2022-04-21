#include "GlobalBrightness.h"

GlobalBrightness::GlobalBrightness() :
  Menu(HSV_YELLOW)
{
}

bool GlobalBrightness::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  return true;
}
