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
  void onShortClick() override;
  void onShortClick2() override;
  void onLongClick() override;
  void onLongClick2() override;

private:
  void showBrightnessSelection();

  // the list of brightness options, this can be
  // any length really and the menu will adjust
  const uint8_t m_brightnessOptions[4] = { 20, 65, 110, 155 };
};

#endif
