#ifndef MENU_H
#define MENU_H

#include <inttypes.h>
#include <string>

#include "../ColorTypes.h"

class Mode;
class Button;
class LedControl;
class TimeControl;

class Menu
{
  public:
    Menu(RGBColor col);
    virtual ~Menu();

    // optional init function can be overridden
    virtual bool init(Mode *curMode);

    // when the menu runs it will have access to time, the button and led control
    virtual bool run(const TimeControl *timeControl, const Button *button, LedControl *ledControl) = 0;

    // optional handlers for clicks
    virtual void onShortClick();
    virtual void onLongClick();

    // get the color of this menu
    RGBColor color() { return m_menuColor; }

  protected:
    // close the current menu
    void leaveMenu() { m_shouldClose = true; }

    // the current mode that was selected
    Mode *m_pCurMode;

  private:
    // whether to close the menu
    bool m_shouldClose;

    // the color of the menu in the menu ring
    RGBColor m_menuColor;
};

#endif
