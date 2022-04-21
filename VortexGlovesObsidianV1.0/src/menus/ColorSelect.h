#ifndef COLOR_SELECT_H
#define COLOR_SELECT_H

#include "Menu.h"

class ColorSelect : public Menu
{
  public:
    ColorSelect();

    bool run(const TimeControl *timeControl, const Button *button, LedControl *ledControl);

  private:

};

#endif
