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
  m_curSelection(QUADRANT_FIRST),
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
  // reset the current selection
  m_curSelection = QUADRANT_FIRST;
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

void Menu::onShortClick2()
{
}

void Menu::onLongClick()
{
  leaveMenu(false);
}

void Menu::onLongClick2()
{
}

void Menu::leaveMenu(bool doSave)
{
  m_shouldClose = true;
  if (doSave) {
    Modes::saveStorage();
  }
  //DEBUG_LOG("Leaving Menu");
}

void Menu::blinkSelection(uint32_t offMs, uint32_t onMs)
{
  uint32_t blinkCol = RGB_OFF;
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    // blink green if long pressing on a selection
    blinkCol = RGB_DIM_WHITE1;
  }
  switch (m_curSelection) {
  case QUADRANT_LAST:
    // exit thumb breathes red on the tip and is either blank or red on the top
    // depending on whether you've held for the short click threshold or not
    if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
      Leds::setQuadrantFive(RGB_WHITE);
    } else {
      Leds::clearQuadrantFive();
      Leds::setQuadrantFive(RGB_RED);
      Leds::blinkQuadrantFive(Time::getCurtime(), 250, 500, RGB_BLANK);
    }
    break;
  case QUADRANT_COUNT:
    // special selection clause 'select all' do nothing
    break;
  default:
    // otherwise just blink the selected finger to off from whatever
    // color or pattern it's currently displaying
    if (blinkCol == RGB_OFF && Leds::getLed(quadrantFirstLed(m_curSelection)).empty()) {
      // if the blink color is 'off' and the led is a blank then we
      // need to blink to a different color
      blinkCol = RGB_BLANK;
    }
    // blink the target finger to the target color
    Leds::blinkQuadrant(m_curSelection,
      g_pButton->isPressed() ? g_pButton->holdDuration() : Time::getCurtime(),
      offMs, onMs, blinkCol);
    break;
  }
}
