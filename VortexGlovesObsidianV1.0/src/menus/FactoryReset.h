#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

#include "Menu.h"

class FactoryReset : public Menu
{
  public:
    FactoryReset();

    bool init(Mode *curMode);

    bool run(const TimeControl *timeControl, const Button *button, LedControl *ledControl);

    // handlers for clicks
    void onShortClick();
    void onLongClick();

  private:

};

#endif
