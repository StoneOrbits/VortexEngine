#ifndef GLOBAL_BRIGHTNESS_H
#define GLOBAL_BRIGHTNESS_H

#include "Menu.h"

class GlobalBrightness : public Menu
{
  public:
    GlobalBrightness();

    bool init(Mode *curMode);

    bool run(const TimeControl *timeControl, const Button *button, LedControl *ledControl);

    // handlers for clicks
    void onShortClick();
    void onLongClick();

  private:

};

#endif
