#ifndef RING_MENU_H
#define RING_MENU_H

#include <inttypes.h>

#include "../Leds/LedTypes.h"

enum MenuEntryID
{
  MENU_NONE = -1,
  MENU_FIRST = 0,

  MENU_RANDOMIZER = MENU_FIRST,
  MENU_COLOR_SELECT,
  MENU_PATTERN_SELECT,
  MENU_GLOBAL_BRIGHTNESS,
  MENU_FACTORY_RESET,
  MENU_MODE_SHARING,
  MENU_EDITOR_CONNECTION,
  // add new menus here

  // the total number of menus
  MENU_COUNT
};

class Menu;

class Menus
{
  // private unimplemented constructor
  Menus();

public:
  // opting for static class here because there should only ever be one
  // Menu control object and I don't like singletons
  static bool init();
  static void cleanup();

  // Run the menus returns true if the menu remains open, false if closed
  static bool run();

  // open the ring menu
  static bool openMenuSelection();

  // open a menu by index in the menu table
  static bool openMenu(uint32_t index);

  // check if the menus are open
  static bool checkOpen();
  static bool checkInMenu();
  static Menu *curMenu();
  static MenuEntryID curMenuID();

private:
  // run the currently open menu
  static bool runCurMenu();
  // run the ring filling logic
  static bool runMenuSelection();
  // helper to calculate the relative hold time for the current menu
  static LedPos calcLedPos();
  // close the currently open menu
  static void closeCurMenu();

  // =====================
  //  private members

  enum MenuState {
    // menus aren't open at all
    MENU_STATE_NOT_OPEN = 0,

    // menus are open and the user is choosing a menu
    MENU_STATE_MENU_SELECTION,

    // the menus are open and we opened a menu
    MENU_STATE_IN_MENU
  };

  // the current menus states
  static MenuState m_menuState;

  // the ring menu section
  static uint8_t m_selection;
  // the time when we first opened the ringmenu
  static uint64_t m_openTime;
  // the current sub menu that is open
  static Menu *m_pCurMenu;
};

#endif
