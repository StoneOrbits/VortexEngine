#include "RingMenu.h"

#include "LedControl.h"
#include "Button.h"

RingMenu::RingMenu() :
  m_randomizer(),
  m_colorSelect(),
  m_patternSelect(),
  m_globalBrightness(),
  m_factoryReset(),
  m_modeSharing(),
  //m_menuList(),
  m_selection(0),
  m_isOpen(false)
{
}

bool RingMenu::init()
{
  // Some sort of auto-registration mechanism for this would be nice
  // but in reality how often are people going to create new Menus
  return true;
}

void RingMenu::registerMenu(Menu *menu, RGBColor color)
{
  //m_menuList.push_back(MenuEntry(menu, color));
}

Menu *RingMenu::run()
{
  // if the button was released this tick and the ringmenu was open 
  // then close the ringmenu and return the current menu selection
  if (g_pButton->onRelease() && m_isOpen) {
    // the menu is no longer open
    m_isOpen = false;
    // return the menu that was selected
    return m_menuList[m_selection].menu;
  }
  // make sure the button is pressed and held for at least one second
  if (!g_pButton->isPressed() || g_pButton->holdDuration() < 1000) {
    // no menu selected yet
    return nullptr;
  }
  // if the ring menu just opened this tick
  if (!m_isOpen) {
    // reset the current selection just in case
    m_selection = 0;
    // the ring menu is now open
    m_isOpen = true;
  }
  // calculate how long into the current menu the button was held
  // this will be a value between 0 and 1000 based on the current
  // menu selection and hold time
  int holdTime = calculateHoldTime();
  // the leds turn on in sequence every 100ms another turns on:
  //  000ms = led 0 to 0
  //  100ms = led 0 to 1
  //  200ms = led 0 to 2
  LedPos led = (LedPos)(holdTime / 100);
  // only try to turn on up till LED_LAST leds
  if (led > LED_LAST) led = LED_LAST;
  // turn on leds LED_FIRST through led with the selected menu's given color
  g_pLedControl->setRange(LED_FIRST, led, m_menuList[m_selection].color);
  // no menu selected yet
  return nullptr;
}

// helper to calculate the relative hold time for the current menu
int RingMenu::calculateHoldTime()
{
  // this allows the menu to wrap around to beginning after the end
  // if the user never lets go of the button
  int holdDuration = g_pButton->holdDuration() % (1000 + (1000 * numMenus()));
  // the threshold for when the current menu starts 1sec + 1sec per menu
  int threshold = 1000 + (1000 * m_selection);
  // the amount of time held in the current menu, should be 0 to 1000 ms
  int holdTime = (holdDuration - threshold);
  // if the holdTime is within 1000 then it's valid
  if (holdTime <= 1000) {
    return holdTime;
  }
  // otherwise increment selection and wrap around at numMenus
  m_selection = (m_selection + 1) % numMenus();
  // then re-calculate the holdTime it should be less than 1000
  return calculateHoldTime();
}
