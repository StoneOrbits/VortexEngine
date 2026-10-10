#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <inttypes.h>

#include "../Colors/ColorTypes.h"
#include "../Leds/LedTypes.h"
#include "../Modes/Mode.h"

// the action for the menu to execute
typedef uint8_t MenuAction;

// quit the menus
#define MENU_QUIT     0
// continue running the menu
#define MENU_CONTINUE 1
// don't run derived menu code, this is used internally
// by the menu class itself
#define MENU_SKIP     2

// vtable for the Menu virtuals; hooks may be NULL
typedef struct MenuVtable
{
  // derived initializer (calls Menu_initBase, returns false on failure)
  bool (*init)(void *self);
  // derived run loop (calls Menu_runBase first, returns MENU_* action)
  MenuAction (*run)(void *self);
  void (*onLedSelected)(void *self);
  void (*onShortClick)(void *self);
  void (*onLongClick)(void *self);
  void (*leaveMenu)(void *self, bool doSave);
  // frees the derived allocation; must call Menu_baseDestroy last
  void (*destroy)(void *self);
} MenuVtable;

typedef struct Menu
{
  // the mode copied from the current mode used to preview changes
  Mode m_previewMode;
  // the color of this menu
  RGBColor m_menuColor;
  // tracks the targetted leds for this menu
  // note this is an led map
  LedMap m_targetLeds;
  // all menus have a 'current selection'
  uint8_t m_curSelection;
  // true once a an led is selected
  bool m_ledSelected;
  // whether advanced menu was activated
  bool m_advanced;
  // internal flag to close the menu
  bool m_shouldClose;
  // the derived menu's hooks
  const MenuVtable *m_vtable;
} Menu;

Menu *Menu_new(const RGBColor *col, bool advanced);
void Menu_initBase(Menu *self, const MenuVtable *vtable);
// destroy the menu, dispatching to the derived destroy when provided
void Menu_destroy(Menu *self);
// the base cleanup that derived destroys must call last
void Menu_baseDestroy(Menu *self);

// initialize the menu, dispatching to the derived init when provided
bool Menu_init(Menu *self);
// the base initializer that derived inits must call
bool Menu_baseInit(Menu *self);

// run the menu, dispatching through the vtable where provided
MenuAction Menu_run(Menu *self);
// the base run loop that derived runs must call
MenuAction Menu_runBase(Menu *self);

// optional handlers for clicks
void Menu_onLedSelected(Menu *self);
void Menu_onShortClick(Menu *self);
void Menu_onLongClick(Menu *self);

// close the current menu
void Menu_leaveMenu(Menu *self, bool doSave);

// the base implementation that derived leaveMenu overrides call into
void Menu_leaveMenuBase(Menu *self, bool doSave);

void Menu_showBulbSelection(Menu *self);
void Menu_showExit(Menu *self);

// iterate to next bulb selection
void Menu_nextBulbSelection(Menu *self);

// skip led selection
void Menu_bypassLedSelection(Menu *self, LedMap map);

#endif