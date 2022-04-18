#ifndef MENU_H
#define MENU_H

#include <inttypes.h>
#include <string>

#include "Color.h"

class Menu
{
  public:
    Menu(const char *name, RGBColor col);
    virtual ~Menu();

    virtual bool run() = 0;

    std::string name() { return m_name; }
    RGBColor color() { return m_menuColor; }

  protected:
    // the name of the menu
    std::string m_name;

    // the color of the menu in the menu ring
    RGBColor m_menuColor;

    // the current menu selection in this menu
    uint32_t m_curSelection;

  private:
};

#endif
