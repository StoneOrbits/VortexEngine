#include "Menus.h"

// menus
#include "Menu.h"

#include "MenuList/EditorConnection.h"
#include "MenuList/GlobalBrightness.h"
#include "MenuList/PatternSelect.h"
#include "MenuList/FactoryReset.h"
#include "MenuList/ModeSharing.h"
#include "MenuList/ColorSelect.h"
#include "MenuList/Randomizer.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Buttons/Button.h"
#include "../Serial/Serial.h"
#include "../Modes/Modes.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

// static members
Menus::MenuState Menus::m_menuState = MENU_STATE_NOT_OPEN;
uint64_t Menus::m_openTime = 0;
uint32_t Menus::m_selection = 0;
Menu *Menus::m_pCurMenu = nullptr;

// entries for the ring menu
typedef Menu *(*initMenuFn_t)();
struct MenuEntry {
  const char *menuName;
  initMenuFn_t initMenu;
  RGBColor color;
};

// a template to initialize ringmenu functions
template <typename T>
Menu *initMenu() { return new T(); }

// a simple macro to simplify the entries in the menu list
#define ENTRY(classname, color) { #classname, initMenu<classname>, color }

// The list of menus that are registered with colors to show in ring menu
const MenuEntry menuList[MENU_COUNT] = {
  ENTRY(Randomizer,       RGB_WHITE),   // 0
  ENTRY(ColorSelect,      RGB_ORANGE),  // 1
  ENTRY(PatternSelect,    RGB_BLUE),    // 2
  ENTRY(GlobalBrightness, RGB_YELLOW),  // 3
  ENTRY(FactoryReset,     RGB_RED),     // 4
  ENTRY(ModeSharing,      RGB_TEAL),    // 5
  ENTRY(EditorConnection, RGB_PURPLE),  // 5
};

// the number of menus in the above array
#define NUM_MENUS (sizeof(menuList) / sizeof(menuList[0]))

bool Menus::init()
{
  m_menuState = MENU_STATE_NOT_OPEN;
  // sub-menus are initialized right before they run
  return true;
}

void Menus::cleanup()
{
  // close any open menu
  if (m_menuState == MENU_STATE_IN_MENU) {
    m_pCurMenu->leaveMenu();
    m_pCurMenu = nullptr;
    m_menuState = MENU_STATE_NOT_OPEN;
  }
  m_selection = 0;
  m_openTime = 0;
}

bool Menus::run()
{
  switch (m_menuState) {
  case MENU_STATE_NOT_OPEN:
  default:
    // make sure the button is pressed and held till the threshold
    if (g_pButton->isPressed() && g_pButton->holdDuration() >= MENU_TRIGGER_THRESHOLD_TICKS) {
      // save the time of when we open the menu so we can fill based on curtime from then
      m_openTime = Time::getCurtime();
      // open the menu
      m_menuState = MENU_STATE_RING_FILL;
      return true;
    }
    // no menu open yet
    return false;
  case MENU_STATE_RING_FILL:
    return runRingFill();
  case MENU_STATE_IN_MENU:
    return runCurMenu();
  }
  // nothing to run
  return false;
}

bool Menus::runRingFill()
{
  if (g_pButton->onShortClick()) {
    // otherwise increment selection and wrap around at num menus
    m_selection = (m_selection + 1) % NUM_MENUS;
    // reset the open time so that it starts again
    m_openTime = Time::getCurtime();
    // clear the leds
    Leds::clearAll();
    return true;
  }
  if (g_pButton2->onShortClick()) {
    // increment down the menu list and wrap at 0
    if (!m_selection) {
      m_selection = MENU_COUNT - 1;
    } else {
      --m_selection;
    }
    Leds::clearAll();
    return true;
  }
  // if the button was long pressed then select this menu, but we
  // need to check the presstime to ensure we don't catch the initial
  // release after opening the ringmenu
  if (g_pButton->onLongClick() && g_pButton->pressTime() >= m_openTime) {
    // ringmenu is open so select the menu
    DEBUG_LOGF("Selected ringmenu %s", menuList[m_selection].menuName);
    // open the menu we have selected
    if (!openMenu(m_selection)) {
      DEBUG_LOGF("Failed to initialize %s menu", menuList[m_selection].menuName);
      return false;
    }
    // display the newly opened menu
    return true;
  }
  // if you long click the 2nd button exit the menu list
  if (g_pButton2->onLongClick()) {
    // close the current menu when run returns false
    closeCurMenu();
    // return false to let the modes play
    return false;
  }
  // clear the leds so it always fills instead of replacing
  Leds::clearAll();
  // calculate how long into the current menu the button was held
  // this will be a value between 0 and LED_COUNT based on the current
  // menu selection and hold time
  // the leds turn on in sequence every tick another turns on:
  //  000ms = led 0 to 0
  //  100ms = led 0 to 1
  //  200ms = led 0 to 2
  LedPos led = calcLedPos();
#if FILL_FROM_THUMB == 1
  // turn on leds from led to LED_LAST because the menu is filling downward
  Leds::setRange(led, LED_LAST, menuList[m_selection].color);
#else
  // turn on leds LED_FIRST through led with the selected menu's given color
  Leds::setRange(LED_FIRST, led, menuList[m_selection].color);
#endif
  // continue in the menu
  return true;
}

bool Menus::runCurMenu()
{
  // first run the click handlers for the menu
  if (g_pButton->onShortClick()) {
    m_pCurMenu->onShortClick();
  }
  if (g_pButton2->onShortClick()) {
    m_pCurMenu->onShortClick2();
  }
  if (g_pButton->onLongClick()) {
    m_pCurMenu->onLongClick();
  }
  if (g_pButton2->onLongClick()) {
    m_pCurMenu->onLongClick2();
  }

  // if the menu run handler returns false that signals the
  // menu was closed by the user leaving the menu
  if (!m_pCurMenu->run()) {
    // close the current menu when run returns false
    closeCurMenu();
    // return false to let the modes play
    return false;
  }
  // the opened menu and don't play modes
  return true;
}

// helper to calculate the relative hold time for the current menu
LedPos Menus::calcLedPos()
{
  // this allows the menu to wrap around to beginning after
  uint32_t holdDuration = (Time::getCurtime() - m_openTime) % MENU_DURATION_TICKS;
  // calcluate the led for the hold duration
  LedPos led = (LedPos)(((double)holdDuration / MENU_DURATION_TICKS) * LED_COUNT);
  // if the holdTime is within MENU_DURATION_TICKS then it's valid
#if FILL_FROM_THUMB == 1
  return (LedPos)(LED_LAST - led);
#else
  return led;
#endif
}

bool Menus::openMenu(uint32_t index)
{
  if (index >= NUM_MENUS) {
    return false;
  }
  Menu *newMenu = menuList[index].initMenu();
  if (!newMenu) {
    return false;
  }
  // initialiaze the new menu with the current mode
  if (!newMenu->init()) {
    DEBUG_LOGF("Failed to initialize %s menu", menuList[index].menuName);
    // if the menu failed to init, don't open it
    delete newMenu;
    return false;
  }
  if (m_pCurMenu) {
    delete m_pCurMenu;
  }
  // assign the new menu
  m_pCurMenu = newMenu;
  // and the menus are open just in case
  // clear all the leds
  Leds::clearAll();
  m_menuState = MENU_STATE_IN_MENU;
  return true;
}

void Menus::closeCurMenu()
{
  if (m_pCurMenu) {
    delete m_pCurMenu;
    m_pCurMenu = nullptr;
  }
  m_menuState = MENU_STATE_NOT_OPEN;
  m_selection = 0;
}
