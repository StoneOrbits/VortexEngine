#ifndef RING_MENU_H
#define RING_MENU_H

#include <stdbool.h>
#include <inttypes.h>

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
  // add new menus here

  // the total number of menus
  MENU_COUNT
};

typedef struct Menu Menu;

typedef uint8_t MenuState;

// menus aren't open at all
#define MENU_STATE_NOT_OPEN        0
// menus are open and the user is choosing a menu
#define MENU_STATE_MENU_SELECTION  1
// the menus are open and we opened a menu
#define MENU_STATE_IN_MENU         2

// the current menus states
extern MenuState Menus_m_menuState;

// the ring menu section
extern uint8_t Menus_m_selection;
// the time when we first opened the ringmenu
extern uint32_t Menus_m_openTime;
// the current sub menu that is open
extern Menu *Menus_m_pCurMenu;

// opting for free functions here because there should only ever be one
// Menu control object and I don't like singletons

// =====================
//  free functions

bool Menus_init();
void Menus_cleanup();

bool Menus_run();

bool Menus_openMenuSelection();

bool Menus_openMenu(uint32_t index, bool advanced);

void Menus_showSelection(RGBColor colval);

bool Menus_checkOpen();

bool Menus_checkInMenu();

Menu *Menus_curMenu();

// grab a block from the menu pool to construct a menu in
void *Menus_allocMenu(void);
// return a menu's block to the pool
void Menus_freeMenu(Menu *menu);

MenuEntryID Menus_curMenuID();

bool Menus_runCurMenu();

bool Menus_runMenuSelection();

LedPos Menus_calcLedPos();

void Menus_closeCurMenu();

#endif