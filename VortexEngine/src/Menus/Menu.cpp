#include "Menu.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Buttons/Button.h"
#include "../Modes/Modes.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

Menu::Menu(const RGBColor &col) :
  m_pCurMode(nullptr),
  m_menuColor(col),
  m_shouldClose(false)
{
}

Menu::~Menu()
{
}

bool Menu::init()
{
  // menu is initialized before being run
  m_pCurMode = Modes::curMode();
  if (!m_pCurMode) {
    // if you enter a menu and there's no modes, it will add an empty one
    if (Modes::numModes() > 0) {
      Leds::setAll(RGB_PURPLE);
      // some kind of serious error
      return false;
    }
    if (!Modes::addMode(PATTERN_BASIC, RGBColor(RGB_OFF))) {
      Leds::setAll(RGB_YELLOW);
      // some kind of serious error
      return false;
    }
    // get the mode
    m_pCurMode = Modes::curMode();
    if (!m_pCurMode) {
      Leds::setAll(RGB_ORANGE);
      // serious error again
      return false;
    }
  }
  // just in case
  m_shouldClose = false;
  return true;
}

Menu::MenuAction Menu::run()
{
  // should close?
  if (m_shouldClose) {
    // reset this boolean
    m_shouldClose = false;
    // yep close
    return MENU_QUIT;
  }
  // ===================================
  //  Perform any core menu logic here
  //  and return MENU_SKIP to override
  //  derived menu logic for any reason
  // ===================================
  // continue as normal
  return MENU_CONTINUE;
}

void Menu::onShortClick()
{
}

void Menu::onLongClick()
{
  leaveMenu(false);
}

void Menu::leaveMenu(bool doSave)
{
  m_shouldClose = true;
  if (doSave) {
    Modes::saveStorage();
  }
  //DEBUG_LOG("Leaving Menu");
}
