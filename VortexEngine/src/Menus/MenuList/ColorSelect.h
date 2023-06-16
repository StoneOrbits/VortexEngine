#ifndef COLOR_SELECT_H
#define COLOR_SELECT_H

#include "../Menu.h"

#include "../../Colors/Colorset.h"

class ColorSelect : public Menu
{
public:
  ColorSelect(const RGBColor &col);
  ~ColorSelect();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
  enum ColorSelectState
  {
    // first run after the menu is entered
    STATE_INIT,
    STATE_PICK_SLOT,
    STATE_PICK_HUE1,
    STATE_PICK_HUE2,
    STATE_PICK_SAT,
    STATE_PICK_VAL
  };

  void showSlotSelection();
  void showSelection(ColorSelectState mode);
  void showFullSet(LedPos target, uint64_t time, uint32_t offMs, uint32_t onMs);

  // the options for saturations
  const uint32_t sats[4] = {
    255,
    170,
    85,
    0
  };

  // the options for values
  const uint32_t vals[4] = {
    255,
    170,
    85,
    0
  };

  // the state of the color select menu
  ColorSelectState m_state;
  // the new color being built via hue, sat then val
  HSVColor m_newColor;
  // a color used to return to earlier levels of the color selection
  HSVColor m_base;
  // A copy of the colorset being changed
  Colorset m_colorset;
  // the target slot in the color slot that was selected
  uint8_t m_targetSlot;
};

#endif
