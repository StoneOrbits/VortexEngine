#ifndef GLOBAL_BRIGHTNESS_H
#define GLOBAL_BRIGHTNESS_H

#include "Menu.h"

class GlobalBrightness : public Menu
{
  public:
    GlobalBrightness();

    bool run(const Button *button, LedControl *ledControl);

  private:

};

#endif
