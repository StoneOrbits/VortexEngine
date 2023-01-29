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
  // show the exit on thumb
  showExit();
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

void Menu::showExit()
{
  Leds::setIndex(THUMB_TIP, RGB_DARK_RED);
  Leds::setIndex(THUMB_TOP, RGB_BLANK);
}
