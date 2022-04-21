#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include "Menu.h"

class ModeSharing : public Menu
{
  public:
    ModeSharing();

    bool run(const TimeControl *timeControl, const Button *button, LedControl *ledControl);

  private:

};

#endif
