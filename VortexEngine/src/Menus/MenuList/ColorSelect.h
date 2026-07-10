#ifndef COLOR_SELECT_H
#define COLOR_SELECT_H

#include "../Menu.h"

#include "../../Colors/Colorset.h"

typedef enum {
  COLOR_SELECT_STATE_INIT,
  COLOR_SELECT_STATE_PICK_SLOT,
  COLOR_SELECT_STATE_PICK_HUE1,
  COLOR_SELECT_STATE_PICK_HUE2,
  COLOR_SELECT_STATE_PICK_SAT,
  COLOR_SELECT_STATE_PICK_VAL
} ColorSelectState;

typedef struct ColorSelectMenu_s {
  Menu base;
  ColorSelectState state;
  HSVColor newColor;
  Colorset colorset;
  uint8_t targetSlot;
  uint8_t targetHue1;
} ColorSelectMenu;

Menu *ColorSelectMenu_Create(RGBColor col, bool advanced);
void ColorSelectMenu_destroy(Menu *self);
bool ColorSelectMenu_init(Menu *self);
MenuAction ColorSelectMenu_run(Menu *self);
void ColorSelectMenu_onLedSelected(Menu *self);
void ColorSelectMenu_onShortClick(Menu *self);
void ColorSelectMenu_onLongClick(Menu *self);

extern const MenuVTable g_colorSelectMenuVTable;

#endif
