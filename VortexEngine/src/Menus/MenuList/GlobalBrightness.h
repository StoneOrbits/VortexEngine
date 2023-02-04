#ifndef GLOBAL_BRIGHTNESS_H
#define GLOBAL_BRIGHTNESS_H

#include "../Menu.h"

class GlobalBrightness : public Menu
{
public:
  GlobalBrightness();

  bool init() override;

  bool run() override;

  // handlers for clicks
  void onShortClick() override;
  void onShortClick2() override;
  void onLongClick() override;
  void onLongClick2() override;

private:
  // the four preset brightness options
  const uint32_t m_brightnessOptions[4] = { 20, 65, 110, 155 };
};

#endif
