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

    // run the ringmenu and return a pointer to the menu
    // that should run based on the user selection
    // will return NULL if user is still holding down
    const Menu *run(const Button *button, LedControl *ledControl);

    // whether the ring menu is open
    bool isOpen() { return m_isOpen; }

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
