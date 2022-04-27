#ifndef RING_MENU_H
#define RING_MENU_H

#include <inttypes.h>

// menus
#include "menus/GlobalBrightness.h"
#include "menus/FactoryReset.h"
#include "menus/ModeSharing.h"
#include "menus/ColorSelect.h"
#include "menus/PatternSelect.h"
#include "menus/Randomizer.h"

class Menu;

class RingMenu
{
public:
  RingMenu();

  bool init();

  // Run the ringmenu and fill colors sequentially till the user releases 
  // on a selection. The returned Menu is the selection, or NULL if the user
  // hasn't selected anything yet.
  Menu *run();

  // whether the ring menu is open
  bool isOpen() { return m_isOpen; }

  // the number of menus in the ring menu
  uint32_t numMenus() const { return (sizeof(m_menuList) / sizeof(m_menuList[0])); }

private:
  // helper to register a menu object with a color for the ringmenu
  void registerMenu(Menu *menu, RGBColor col);
  // helper to calculate the relative hold time for the current menu
  LedPos calcLedPos();

  // private structure for menu entry menu => color
  struct MenuEntry
  {
    MenuEntry(Menu *m, RGBColor c) :
      menu(m), color(c)
    {
    }
    Menu *menu;
    RGBColor color;
  };

  // ======================
  //  Menus
  Randomizer m_randomizer;
  ColorSelect m_colorSelect;
  PatternSelect m_patternSelect;
  GlobalBrightness m_globalBrightness;
  FactoryReset m_factoryReset;
  ModeSharing m_modeSharing;

  // list of menu entries above with chosen colors
  const MenuEntry m_menuList[6] = {
    { &m_randomizer, RGB_WHITE },
    { &m_colorSelect, RGB_ORANGE },
    { &m_patternSelect, RGB_BLUE },
    { &m_globalBrightness, RGB_YELLOW },
    { &m_factoryReset, RGB_RED },
    { &m_modeSharing, RGB_TEAL },
  };

  // =====================
  //  private members

  // the ring menu section
  uint32_t m_selection;

  // whether the ring menu is open
  bool m_isOpen;
};

#endif
