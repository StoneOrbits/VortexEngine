#ifndef COLOR_SELECT_H
#define COLOR_SELECT_H

#include "Menu.h"

#include "../Colorset.h"

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
  void showQuadSelection();
  void showHueSelection();
  void showSatSelection();
  void showValSelection();

  // Generate hue/sat/val values based on selection rules
  uint32_t makeQuad(uint32_t quad);
  uint32_t makeHue(uint32_t quad, uint32_t selection);
  uint32_t makeSat(uint32_t selection);
  uint32_t makeVal(uint32_t selection);

  // private enumeration for internal state of color selection
  enum ColorSelectState : uint32_t
  {
    // currently picking the color slot to change
    STATE_PICK_SLOT,
    // currently picking the quadrant of color
    STATE_PICK_QUAD,
    // picking a hue within the quadrant
    STATE_PICK_HUE,
    // picking a saturation for the color
    STATE_PICK_SAT,
    // picking a value for the color
    STATE_PICK_VAL,
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
