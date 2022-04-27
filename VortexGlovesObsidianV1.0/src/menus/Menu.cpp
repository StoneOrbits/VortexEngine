#include "Menu.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Button.h"

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

bool Menu::run()
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

void Menu::leaveMenu()
{
  m_shouldClose = true;
  //Debug("Leaving Menu");
}

void Menu::blinkSelection()
{
  // only blink off for 250ms per second
  if ((g_pTimeControl->getCurtime() % 1000) < 250) {
    return;
  }
  RGBColor blinkCol = RGB_OFF;
  // blinkie red when held past long-press threhold
  if (g_pButton->isPressed() && g_pButton->holdDuration() > 250) {
    blinkCol = RGBColor(150, 10, 10);
  }
  // special selection clause 'select all'
  if (m_curSelection == FINGER_COUNT) {
    g_pLedControl->setAll(blinkCol);
  } else {
    // otherwise just blink the selected finger
    g_pLedControl->setFinger(m_curSelection, blinkCol);
  }
}
