#include "Menu.h"

Menu::Menu() :
  m_pCurMode(nullptr),
  m_curSelection(0),
  m_shouldClose(false)
{
}

Menu::~Menu()
{
}

bool Menu::init(Mode *curMode)
{
  // menu is initialized before being run
  m_pCurMode = curMode;
  // reset the current selection
  m_curSelection = 0;
  // just in case
  m_shouldClose = false;
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
}
