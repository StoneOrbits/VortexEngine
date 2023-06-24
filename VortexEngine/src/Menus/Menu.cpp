#include "Menu.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Buttons/Button.h"
#include "../Modes/Modes.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

Menu::Menu(const RGBColor &col, bool advanced) :
  m_pCurMode(nullptr),
  m_menuColor(col),
  m_targetLeds(MAP_LED_ALL),
  m_curSelection(FINGER_FIRST),
  m_ledSelected(false),
  m_advanced(advanced),
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
      // some kind of serious error
      return false;
    }
    if (!Modes::addMode(PATTERN_BASIC, RGBColor(RGB_OFF))) {
      // some kind of serious error
      return false;
    }
    // get the mode
    m_pCurMode = Modes::curMode();
    if (!m_pCurMode) {
      // serious error again
      return false;
    }
  }
  // reset the current selection
  m_curSelection = FINGER_FIRST;
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
    nextBulbSelection();
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
  if (m_targetLeds == MAP_LED(LED_MULTI)) {
    LedPos pos = (LedPos)((Time::getCurtime() / 30) % LED_COUNT);
    Leds::blinkIndex(pos, Time::getCurtime() + (pos * 10), 50, 500, m_menuColor);
  } else {
    Leds::blinkMap(m_targetLeds, Time::getCurtime(), 250, 500, m_menuColor);
  }
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
    Leds::setAll(RGB_DIM_WHITE2);
  }
}

void Menu::showExit()
{
  Leds::setIndex(THUMB_TIP, RGB_DARK_RED);
  Leds::setIndex(THUMB_TOP, RGB_BLANK);
}

void Menu::nextBulbSelection()
{
  // The target led can be 0 through LED_COUNT to represent any led or all leds
  // modulo by LED_COUNT + 1 to include LED_COUNT (all) as a target
  switch (m_targetLeds) {
  case MAP_LED_ALL:
    if (m_pCurMode->isMultiLed()) {
      // do not allow multi led to select anything else
      //break;
    }
    m_targetLeds = MAP_LED(LED_FIRST);
    break;
  case MAP_LED(LED_LAST):
    m_targetLeds = MAP_PAIR_EVENS;
    break;
  case MAP_PAIR_EVENS:
    m_targetLeds = MAP_PAIR_ODDS;
    break;
  case MAP_PAIR_ODDS:
    m_targetLeds = MAP_LED(LED_MULTI);
    break;
  case MAP_LED(LED_MULTI):
    m_targetLeds = MAP_LED_ALL;
    break;
  default: // LED_FIRST through LED_LAST
    // do not allow multi led to select anything else
    if (m_pCurMode->isMultiLed()) {
      //m_targetLeds = MAP_LED_ALL;
      //break;
    }
    // iterate as normal
    m_targetLeds = MAP_LED(((mapGetFirstLed(m_targetLeds) + 1) % (LED_COUNT + 1)));
    break;
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

void Menu::blinkSelection(uint32_t offMs, uint32_t onMs)
{
  uint32_t blinkCol = RGB_OFF;
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    // blink green if long pressing on a selection
    blinkCol = RGB_DIM_WHITE1;
  }
  switch (m_curSelection) {
  case FINGER_THUMB:
    // exit thumb breathes red on the tip and is either blank or red on the top
    // depending on whether you've held for the short click threshold or not
    Leds::breathIndex(THUMB_TIP, 250, (uint32_t)(Time::getCurtime() / 2), 10, 255, 180);
    if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
      Leds::setIndex(THUMB_TOP, RGB_RED);
    } else {
      Leds::clearIndex(THUMB_TOP);
      Leds::blinkIndex(THUMB_TOP, Time::getCurtime(), 250, 500, RGB_BLANK);
    }
    break;
  case FINGER_COUNT:
    // special selection clause 'select all' do nothing
    break;
  default:
    // otherwise just blink the selected finger to off from whatever
    // color or pattern it's currently displaying
    if (blinkCol == RGB_OFF && Leds::getLed(fingerTip(m_curSelection)).empty()) {
      // if the blink color is 'off' and the led is a blank then we
      // need to blink to a different color
      blinkCol = RGB_BLANK;
    }
    // blink the target finger to the target color
    Leds::blinkIndex(fingerTip(m_curSelection),
                      g_pButton->isPressed() ? g_pButton->holdDuration() : Time::getCurtime(),
                      offMs, onMs, blinkCol);
    break;
  }
}
