#include "Menu.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Buttons/Button.h"
#include "../Modes/Modes.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

Menu::Menu() :
  m_pCurMode(nullptr),
  m_curSelection(FINGER_FIRST),
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
    // need a mode for menus to operate on
    return false;
  }
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
  // render all the options
  if (!showOptions()) {
    // stop running menu?
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
  RGBColor blinkCol = RGB_OFF;
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    // blink green if long pressing on a selection
    blinkCol = RGBColor(0, 200, 0);
  }
  // thumb should always be off unless it's blinking to red
  Leds::clearFinger(FINGER_THUMB);
  switch (m_curSelection) {
  case FINGER_THUMB:
    // exit on thumb blink off/red
    //Leds::blinkFinger(FINGER_THUMB, 250, 500, RGB_RED);
    break;
  case FINGER_COUNT:
    // special selection clause 'select all'
    Leds::clearAll();
    //Leds::blinkAll(offMs, onMs, blinkCol);
    break;
  default:
    // otherwise just blink the selected finger to off from whatever
    // color or pattern it's currently displaying
    //Leds::blinkFinger(m_curSelection, offMs, onMs, blinkCol);
    break;
  }
}

bool Menu::showOptions() const
{
  OptionIndex idx = OptionIndex::MENU_OPTION_1;
  while (idx < OptionIndex::MENU_OPTION_COUNT) {
    Leds::setIndex(fingerTip((Finger)idx), RGB_WHITE);
    Leds::setIndex(fingerTop((Finger)idx), m_options[idx].color);
    idx = (OptionIndex)(idx + 1);
  }
  return true;
}

void Menu::setOption(RGBColor col)
{
  m_options[option].color = col;
}

void Menu::setOptions(uint32_t numOptions, const RGBColor *cols)
{
  for (uint32_t opt = 0; opt < numOptions; ++opt) {
    addOption(cols[opt]);
  }
}
