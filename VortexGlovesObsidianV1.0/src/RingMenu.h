#ifndef RING_MENU_H
#define RING_MENU_H

#include <inttypes.h>
#include <vector>

class LedControl;
class Button;
class Menu;

class RingMenu
{
  public:
    RingMenu();

    bool init();

    // Run the ringmenu and fill colors sequentially till the user releases 
    // on a selection. The returned Menu is the selection, or NULL if the user
    // hasn't selected anything yet.
    Menu *run(const Button *button, LedControl *ledControl);

    // whether the ring menu is open
    bool isOpen() { return m_isOpen; }

    // the number of menus in the ring menu
    uint32_t numMenus() { return m_menuList.size(); }

  private:
    // whether the ring menu is open
    bool m_isOpen;

    // the ring menu section
    uint32_t m_selection;

    // current menu that is open
    Menu *m_pCurMenu;

    // list of menu entries
    std::vector<Menu *> m_menuList;
};

#endif
