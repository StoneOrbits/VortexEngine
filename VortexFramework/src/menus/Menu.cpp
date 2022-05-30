#include "Menu.h"

#include "../TimeControl.h"
#include "../Timings.h"
#include "../Button.h"
#include "../Modes.h"
#include "../Leds.h"
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

void Menu::leaveMenu()
{
  m_shouldClose = true;
  //Debug("Leaving Menu");
}

void Menu::blinkSelection(uint32_t offMs, uint32_t onMs)
{
  RGBColor blinkCol = RGB_OFF;
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD) {
    // blink green if long pressing on a selection
    blinkCol = RGBColor(0, 200, 0);
  }
  // thumb should always be off unless it's blinking to red
  Leds::clearFinger(FINGER_THUMB);
  switch (m_curSelection) {
  case FINGER_THUMB:
    // exit on thumb blink off/red
    Leds::blinkFinger(FINGER_THUMB, 250, 500, RGB_RED);
    break;
  case FINGER_COUNT:
    // special selection clause 'select all'
    Leds::clearAll();
    Leds::blinkAll(offMs, onMs, blinkCol);
    break;
  default:
    // otherwise just blink the selected finger to off from whatever
    // color or pattern it's currently displaying
    Leds::blinkFinger(m_curSelection, offMs, onMs, blinkCol);
    break;
  }
}
