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
  m_targetLed(LED_0),
  m_ledSelected(false),
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
  // check whether the close flag has been set
  if (m_shouldClose) {
    // reset flag and close
    m_shouldClose = false;
    return MENU_QUIT;
  }

  // if the led has already been selected continue as normal
  // and allow derived menu logic to perform work
  if (m_ledSelected) {
    return MENU_CONTINUE;
  }

  // using manual onShortClick and onLongClick check here because
  // there is no guarantee the child class will call the parent
  // class's onShortClick and onLongClick functions so

  // every time the button is clicked, change the target led
  if (g_pButton->onShortClick()) {
    // The target led can be 0 through LED_COUNT to represent any led or all leds
    // modulo by LED_COUNT + 1 to include LED_COUNT (all) as a target
    m_targetLed = (LedPos)((m_targetLed + 1) % (LED_COUNT + 1));
  }
  // on a long press of the button, lock in the target led
  if (g_pButton->onLongClick()) {
    m_ledSelected = true;
  }

  // render the bulb selection
  showBulbSelection();

  // return MENU_SKIP to indicate to derived menu classes that
  // they should skip their logic and just return so that the
  // bulb selection can be rendered
  return MENU_SKIP;
}

void Menu::showBulbSelection()
{
  Leds::clearAll();
  Leds::setIndex(m_targetLed, m_menuColor);
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
