#ifndef MENU_H
#define MENU_H

#include <inttypes.h>
#include <string>

#include "../ColorTypes.h"

class Button;
class LedControl;

class Menu
{
  public:
    Menu(RGBColor col);
    virtual ~Menu();

    // when the menu runs it will have access to the button and led control
    virtual bool run(const Button *button, LedControl *ledControl) = 0;

    // get the color of this menu
    RGBColor color() { return m_menuColor; }

  protected:
    // the current menu selection in this menu
    uint32_t m_curSelection;

  private:
    // the color of the menu in the menu ring
    RGBColor m_menuColor;
};

#endif
