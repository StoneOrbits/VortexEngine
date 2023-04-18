#include "Menu.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Buttons/Button.h"
#include "../Modes/Modes.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

Menu::Menu(const RGBColor &col) :
  m_pCurMode(nullptr),
  m_menuColor(col),
  m_targetLeds(MAP_LED_ALL),
  m_ledSelected(false),
  m_curSelection(0),
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
    switch (m_targetLeds) {
    case MAP_LED_ALL:
      if (m_pCurMode->isMultiLed()) {
        // do not allow multi led to select anything else
        break;
      }
      m_targetLeds = MAP_LED(LED_FIRST);
      break;
    case MAP_LED(LED_LAST):
      m_targetLeds = MAP_LED_ALL;
      break;
    default: // LED_FIRST through LED_LAST
      // do not allow multi led to select anything else
      if (m_pCurMode->isMultiLed()) {
        m_targetLeds = MAP_LED_ALL;
        break;
      }
      // iterate as normal
      m_targetLeds = MAP_LED(((mapGetFirstLed(m_targetLeds) + 1) % (LED_COUNT + 1)));
      break;
    }
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
  Leds::blinkMap(m_targetLeds, Time::getCurtime(), 250, 500, m_menuColor);
  // blink when selecting
  showSelect();
}

void Menu::showSelect()
{
  // blink the tip led white for 150ms when the short
  // click threshold has been surpassed
  if (g_pButton->isPressed() &&
    g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS &&
    g_pButton->holdDuration() < (SHORT_CLICK_THRESHOLD_TICKS + Time::msToTicks(250))) {
    Leds::setAll(RGB_DIM_WHITE1);
  }
}

void Menu::showExit()
{

  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    Leds::setIndex(LED_1, RGB_RED);
  } else {
    Leds::clearIndex(LED_1);
    Leds::blinkIndex(LED_0, Time::getCurtime(), 250, 500, RGB_BLANK);
    Leds::blinkIndex(LED_1, Time::getCurtime(), 250, 500, RGB_DARK_RED);
  }
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
}
