#include "Menu.h"

Menu::Menu(const char *name, CHSV col) :
  m_name(name),
  m_menuColor(col),
  m_curSelection(0)
{
}

Menu::~Menu()
{
}
