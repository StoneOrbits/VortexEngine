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
  uint32_t blinkCol = RGB_OFF;
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    // blink green if long pressing on a selection
    blinkCol = RGB_WHITE;
  }
  // if pressed we blink based on how long we pressed
  uint64_t blinkTime = g_pButton->isPressed() ? g_pButton->holdDuration() : Time::getCurtime();
  // thumb should always be off unless it's blinking to red
  Leds::clearFinger(FINGER_THUMB);
  switch (m_curSelection) {
  case FINGER_THUMB:
    // exit on thumb blink off/red
    Leds::blinkIndex(fingerTop(FINGER_THUMB), blinkTime, 250, 500, RGB_RED);
    Leds::blinkIndex(fingerTip(FINGER_THUMB), blinkTime, 250, 500, RGB_BLANK);
    break;
  case FINGER_COUNT:
    // special selection clause 'select all'
    Leds::clearAll();
    Leds::blinkAll(blinkTime, offMs, onMs, blinkCol);
    break;
  default:
    // otherwise just blink the selected finger to off from whatever
    // color or pattern it's currently displaying
    if (blinkCol == RGB_OFF && Leds::getLed(fingerTip(m_curSelection)).empty()) {
      blinkCol = RGB_BLANK;
    }
    Leds::blinkFinger(m_curSelection, blinkTime, offMs, onMs, blinkCol);
    break;
  }
}
