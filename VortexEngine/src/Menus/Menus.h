#ifndef MENUS_H
#define MENUS_H

#include <inttypes.h>
#include <stdbool.h>

#include "../Colors/ColorTypes.h"
#include "../Leds/LedTypes.h"
#include "../Modes/Mode.h"

enum MenuEntryID
{
  MENU_NONE = -1,
  MENU_FIRST = 0,

  MENU_RANDOMIZER = MENU_FIRST,
  MENU_MODE_SHARING,
#if ENABLE_EDITOR_CONNECTION == 1
  MENU_EDITOR_CONNECTION,
#endif
  MENU_COLOR_SELECT,
  MENU_PATTERN_SELECT,
  MENU_GLOBAL_BRIGHTNESS,
  MENU_FACTORY_RESET,

  MENU_COUNT
};

typedef struct Menu Menu;

bool Menus_init(void);
void Menus_cleanup(void);

bool Menus_run(void);

bool Menus_openMenuSelection(void);

bool Menus_openMenu(uint32_t index, bool advanced);

void Menus_showSelection(RGBColor colval);

bool Menus_checkOpen(void);
bool Menus_checkInMenu(void);
Menu *Menus_curMenu(void);
enum MenuEntryID Menus_curMenuID(void);

#endif
