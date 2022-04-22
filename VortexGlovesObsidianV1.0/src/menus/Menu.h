#ifndef MENU_H
#define MENU_H

#include <inttypes.h>
#include <string>

#include "../ColorTypes.h"

class Button;
class LedControl;
class TimeControl;

class Menu
{
  public:
    Menu(RGBColor col);
    virtual ~Menu();

    // optional init function can be overridden
    virtual bool init();

    // when the menu runs it will have access to time, the button and led control
    virtual bool run(const TimeControl *timeControl, const Button *button, LedControl *ledControl) = 0;

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
