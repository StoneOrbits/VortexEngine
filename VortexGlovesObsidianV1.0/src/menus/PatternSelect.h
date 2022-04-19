#ifndef PATTERN_SELECT_H
#define PATTERN_SELECT_H

#include "Menu.h"

class PatternSelect : public Menu
{
  public:
    PatternSelect();

    bool run(const Button *button, LedControl *ledControl);

  private:

};

#endif
