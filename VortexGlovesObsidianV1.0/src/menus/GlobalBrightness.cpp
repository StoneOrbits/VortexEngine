#include "GlobalBrightness.h"

GlobalBrightness::GlobalBrightness() :
  Menu(HSV_YELLOW)
{
}

bool GlobalBrightness::run(const Button *button, LedControl *ledControl)
{
  return true;
}
