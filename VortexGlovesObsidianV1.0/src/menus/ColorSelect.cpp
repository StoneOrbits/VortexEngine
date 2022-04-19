#include "ColorSelect.h"

ColorSelect::ColorSelect() :
  Menu(HSV_ORANGE)
{

}

bool ColorSelect::run(const Button *button, LedControl *ledControl)
{
  return true;
}
