#include "Menu.h"

Menu::Menu(RGBColor col) :
  m_shouldClose(false),
  m_menuColor(col)
{
}

Menu::~Menu()
{
}

bool Menu::init()
{
  return true;
}

bool Menu::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  // should close?
  if (m_shouldClose) {
    // reset this boolean
    m_shouldClose = false;
    // yep close
    return false;
  }
  // continue as normal
  return true;
}

void Menu::onShortClick()
{
}

void Menu::onLongClick()
{
  // exit the menu
  leaveMenu();
}
