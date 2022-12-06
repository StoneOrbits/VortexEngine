#ifndef RING_MENU_H
#define RING_MENU_H

#include <inttypes.h>

#include "../Leds/LedTypes.h"

enum MenuEntries {
  MENU_RANDOMIZER,
  MENU_COLOR_SELECT,
  MENU_PATTERN_SELECT,
  MENU_GLOBAL_BRIGHTNESS,
  MENU_FACTORY_RESET,
  MENU_MODE_SHARING,

  // add new menus here

  // this one isn't directly accessed, but instead
  // will automatically open when you connect serial
  MENU_EDITOR_CONNECTION,

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

  // Run the ringmenu and any menus it contains
  // returns true if the menu remains open, false if closed
  static bool run();

  // open a menu by index in the menu table
  static bool openMenu(uint32_t index);

private:
  // run the currently open menu
  static bool runCurMenu();
  // run the ring filling logic
  static bool runRingFill();
  // helper to calculate the relative hold time for the current menu
  static LedPos calcLedPos();
  // whether any menus are open
  static bool shouldRun();
  // close the currently open menu
  static void closeCurMenu();

  // =====================
  //  private members

  // the ring menu section
  static uint32_t m_selection;
  // whether the ring menu is open
  static bool m_isOpen;
  // the current sub menu that is open
  static Menu *m_pCurMenu;
};

#endif
