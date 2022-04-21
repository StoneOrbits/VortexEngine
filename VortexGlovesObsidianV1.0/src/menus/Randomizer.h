#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "Menu.h"

class Randomizer : public Menu
{
  public:
    Randomizer();

    bool run(const TimeControl *timeControl, const Button *button, LedControl *ledControl);

  private:

};

#endif
