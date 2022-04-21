#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

#include "Menu.h"

class FactoryReset : public Menu
{
  public:
    FactoryReset();

    bool run(const TimeControl *timeControl, const Button *button, LedControl *ledControl);

  private:

};

#endif
