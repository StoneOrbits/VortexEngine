#include "Menu.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Timings.h"
#include "../Button.h"
#include "../Log.h"

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

void Menu::blinkSelection(uint32_t offMs)
{
  RGBColor blinkCol = RGB_OFF;
  // blinkie red when held past long-press threhold
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD) {
    blinkCol = RGBColor(0, 200, 0);
  }
  // special selection clause 'select all'
  if (m_curSelection == FINGER_COUNT) {
    Leds::blinkAll(offMs, blinkCol);
  } else {
    // otherwise just blink the selected finger
    Leds::blinkFinger(m_curSelection, offMs, blinkCol);
  }
}
