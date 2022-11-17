#ifndef COLOR_SELECT_H
#define COLOR_SELECT_H

#include "../Menu.h"

#include "../../Colors/Colorset.h"

class ColorSelect : public Menu
{
public:
  ColorSelect();

  bool init();

  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  // internal routines for the color select
  void showSlotSelection();
  void showHueSelection1();
  void showHueSelection2();
  void showSatSelection();
  void showValSelection();

  // Generate hue/sat/val values based on selection rules
  uint32_t genValue(uint32_t start, uint32_t divisions, uint32_t amount);

  // overridden blink logic for the colorselect menu (Controls how m_curSelection blinks)
  void blinkSelection(uint32_t offMs = 350, uint32_t onMs = 500) override;

  // private enumeration for internal state of color selection
  enum ColorSelectState : uint32_t
  {
    // currently picking the color slot to change
    STATE_PICK_SLOT,

    // first pick a quadrant 0, 90, 180, 240
    STATE_PICK_HUE1,

    // next pick a quadrant within that quadrant 0, 25, 50, 70
    STATE_PICK_HUE2,

    // picking a saturation for the color
    STATE_PICK_SAT,

    // picking a value for the color
    STATE_PICK_VAL,
  };

  // the options for saturations
  const uint32_t sats[4] = {
    255,
    210,
    180,
    150
  };

  // the options for values
  const uint32_t vals[4] = {
    255,
    210,
    180,
    150
  };

  // the current state of the color selection menu
  ColorSelectState m_state;

  // A copy of the colorset from the current mode
  Colorset m_colorset;

  // the colorselect has multiple pages
  uint32_t m_curPage;

  // the target color slot to change
  uint32_t m_slot;

  // the chosen quadrant
  uint32_t m_quadrant;

  // the new color to set
  HSVColor m_newColor;
};

#endif
