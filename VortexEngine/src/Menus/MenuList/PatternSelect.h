#ifndef PATTERN_SELECT_H
#define PATTERN_SELECT_H

#include "../Menu.h"

#include "../../Colors/Colorset.h"
#include "../../Modes/Mode.h"

typedef struct PatternSelectMenu_s {
  Menu base;
  LedPos srcLed;
  bool started;
} PatternSelectMenu;

Menu *PatternSelectMenu_Create(RGBColor col, bool advanced);
void PatternSelectMenu_destroy(Menu *self);
bool PatternSelectMenu_init(Menu *self);
MenuAction PatternSelectMenu_run(Menu *self);
void PatternSelectMenu_onLedSelected(Menu *self);
void PatternSelectMenu_onShortClick(Menu *self);
void PatternSelectMenu_onLongClick(Menu *self);

extern const MenuVTable g_patternSelectMenuVTable;

#endif
