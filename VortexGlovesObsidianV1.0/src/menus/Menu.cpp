#include "Menu.h"

Menu::Menu(RGBColor col) :
  m_curSelection(0),
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
