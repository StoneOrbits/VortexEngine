#ifndef GLOBAL_BRIGHTNESS_H
#define GLOBAL_BRIGHTNESS_H

#include "Menu.h"

class GlobalBrightness : public Menu
{
  public:
    GlobalBrightness();

    bool init(Mode *curMode);

    bool run();

    // handlers for clicks
    void onShortClick();
    void onLongClick();

  private:
    const uint32_t m_brightnessOptions[4] = { 50, 120, 185, 255 };
};

#endif
