#include "PatternSelect.h"

PatternSelect::PatternSelect() :
  Menu(HSV_BLUE)
{
}

bool PatternSelect::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  return true;
}
