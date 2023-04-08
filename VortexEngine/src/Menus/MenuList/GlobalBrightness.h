#ifndef GLOBAL_BRIGHTNESS_H
#define GLOBAL_BRIGHTNESS_H

#include "../Menu.h"

class GlobalBrightness : public Menu
{
public:
  GlobalBrightness(const RGBColor &col);
  ~GlobalBrightness();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  //void onShortClick() override;
  //void onLongClick() override;

private:
  // the four preset brightness options
  const uint32_t m_brightnessOptions[4] = { 40, 120, 185, 255 };
};

#endif
