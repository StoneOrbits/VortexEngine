#include "Menu.h"

#include "../TimeControl.h"
#include "../LedControl.h"

Menu::Menu() :
  m_pCurMode(nullptr),
  m_curSelection(FINGER_FIRST),
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
  m_curSelection = FINGER_FIRST;
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

void Menu::blinkSelection(const TimeControl *timeControl, LedControl *ledControl)
{
  // only blink off for 250ms per second
  if ((timeControl->getCurtime() % 1000) < 750) {
    return;
  }
  ledControl->clearFinger(m_curSelection);
}
