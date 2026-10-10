#ifndef COLOR_SELECT_H
#define COLOR_SELECT_H

#include <stdbool.h>
#include "../Menu.h"

#include "../../Colors/Colorset.h"

// private enumeration for internal state of color selection
typedef uint8_t ColorSelectState;
enum {
  STATE_INIT,
  STATE_PICK_SLOT,
  STATE_PICK_HUE1,
  STATE_PICK_HUE2,
  STATE_PICK_SAT,
  STATE_PICK_VAL
};

typedef struct ColorSelect
{
  Menu m_base;
  // the state of the color select menu
  ColorSelectState m_state;
  // the new color being built via hue, sat then val
  HSVColor m_newColor;
  // A copy of the colorset being changed
  Colorset m_colorset;
  // the target values selected at each level to build the color, the value
  // selected at the last level isn't stored because you can't go back after
  uint8_t m_targetSlot;
  uint8_t m_targetHue1;
} ColorSelect;

extern const MenuVtable ColorSelect_vtable;

ColorSelect *ColorSelect_new(const RGBColor *col, bool advanced);
void ColorSelect_destroy(ColorSelect *self);

bool ColorSelect_init(ColorSelect *self);
MenuAction ColorSelect_run(ColorSelect *self);

// callback after the user selects the target led
void ColorSelect_onLedSelected(ColorSelect *self);

// handlers for clicks
void ColorSelect_onShortClick(ColorSelect *self);
void ColorSelect_onLongClick(ColorSelect *self);

void ColorSelect_showSlotSelection(ColorSelect *self);
void ColorSelect_showSelection(ColorSelect *self, ColorSelectState mode);
void ColorSelect_showFullSet(ColorSelect *self, uint8_t offMs, uint8_t onMs);

#endif