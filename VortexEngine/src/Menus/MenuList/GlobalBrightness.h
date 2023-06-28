#ifndef GLOBAL_BRIGHTNESS_H
#define GLOBAL_BRIGHTNESS_H

#include "../Menu.h"

#include "../../Modes/Mode.h"

class GlobalBrightness : public Menu
{
public:
  GlobalBrightness(const RGBColor &col, bool advanced);
  ~GlobalBrightness();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
  void showBrightnessSelection();

  // the list of brightness options, this can be
  // any length really and the menu will adjust
  const uint8_t m_brightnessOptions[4] = { 40, 120, 185, 255 };

private:
  // don't worry about this stuff
  enum inova_state : uint8_t
  {
    // sleeping / fake off
    INOVA_STATE_OFF = 0,

    // solid/tracer
    INOVA_STATE_SOLID,

    // dops blink 4 / 16
    INOVA_STATE_DOPS,

    // signal blink 16 / 120
    INOVA_STATE_SIGNAL,

    // total states
    INOVA_STATE_COUNT
  };

  inova_state m_inovaState;
  uint32_t m_lastStateChange;
  uint8_t m_colorIndex;
  Mode m_inovaMode;

  void setInovaState(inova_state newState);
  Menu::MenuAction runInova();
};

#endif
