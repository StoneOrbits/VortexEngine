#include "Menus.h"

// menus
#include "menus/GlobalBrightness.h"
#include "menus/FactoryReset.h"
#include "menus/ModeSharing.h"
#include "menus/ColorSelect.h"
#include "menus/PatternSelect.h"
#include "menus/Randomizer.h"

#include "TimeControl.h"
#include "Timings.h"
#include "Button.h"
#include "Serial.h"
#include "Modes.h"
#include "Leds.h"
#include "Log.h"

// comment this out if you want the menu to fill from pinkie
// the logic is cleaner for fill from pinkie
#define FILL_FROM_THUMB

// static members
uint32_t Menus::m_selection = 0;
bool Menus::m_isOpen = false;
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
};

// the number of menus in the above array
#define NUM_MENUS (sizeof(menuList) / sizeof(menuList[0]))

bool Menus::init()
{
  // sub-menus are initialized right before they run
  return true;
}

void Menus::cleanup()
{
  // not much to cleanup
}

bool Menus::run()
{
  if (!shouldRun()) {
    return false;
  }
  // if there is already a sub-menu open, run that
  if (m_pCurMenu) {
    // run just that menu
    return runCurMenu();
  }
  // otherwise just handle the filling logic
  return runRingFill();
}

bool Menus::runRingFill()
{
  // if the button was released this tick and the ringmenu was open 
  // then close the ringmenu and return the current menu selection
  if (g_pButton->onRelease() && m_isOpen) {
    DEBUG_LOGF("Released on ringmenu %s", menuList[m_selection].menuName);
    // open the menu we have selected
    if (!openMenu(m_selection)) {
      DEBUG_LOGF("Failed to initialize %s menu", menuList[m_selection].menuName);
      return false;
    }
    // continue displaying the menu
    return true;
  }
  // make sure the button is pressed and held for at least one second
  if (!g_pButton->isPressed() || g_pButton->holdDuration() < MENU_TRIGGER_THRESHOLD) {
    // no menu selected yet
    return false;
  }
  // if the menus just opened this tick
  if (!m_isOpen) {
    // reset the current selection just in case
    m_selection = 0;
    // the menus are now open
    m_isOpen = true;
    DEBUG_LOG("Opened RingMenu");
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
#ifdef FILL_FROM_THUMB
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
  if (g_pButton->onLongClick()) {
    m_pCurMenu->onLongClick();
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
  uint32_t relativeHoldDur = g_pButton->holdDuration() - MENU_TRIGGER_THRESHOLD;
  if (g_pButton->holdDuration() < MENU_TRIGGER_THRESHOLD) {
    relativeHoldDur = 0;
  }
  // this allows the menu to wrap around to beginning after the end
  // if the user never lets go of the button
  uint32_t holdDuration = relativeHoldDur % (MENU_DURATION * NUM_MENUS);
  // the time when the current menu starts trigger threshold + duration per menu
  uint32_t menuStartTime = MENU_DURATION * m_selection;
  if (holdDuration >= menuStartTime) {
    // the amount of time held in the current menu, should be 0 to MENU_DURATION ticks
    uint32_t holdTime = (holdDuration - menuStartTime);
    // if the holdTime is within MENU_DURATION then it's valid
    if (holdTime < MENU_DURATION) {
#ifdef FILL_FROM_THUMB
      return (LedPos)(LED_LAST - (((double)holdTime / MENU_DURATION) * LED_COUNT));
#else
      return (LedPos)(((double)holdTime / MENU_DURATION) * LED_COUNT);
#endif
    }
  }
  // otherwise increment selection and wrap around at num menus
  m_selection = (m_selection + 1) % NUM_MENUS;
  // then re-calculate the holdTime it should be less than 10
  return calcLedPos();
}

bool Menus::shouldRun()
{
  // run the menus if they are open or the button is pressed
  return m_isOpen || g_pButton->isPressed();
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
  m_isOpen = true;
  // clear all the leds
  Leds::clearAll();
  // check for serial communications when entering and leaving menus because there's 
  // not much other times to do it and it will take a short delay. If we do it each 
  // step then it creates a visual effect on the LEDs
  SerialComs::checkSerial();
  return true;
}

void Menus::closeCurMenu()
{
  if (m_pCurMenu) {
    delete m_pCurMenu;
    m_pCurMenu = nullptr;
  }
  m_isOpen = false;
  // check for serial communications when entering and leaving menus because there's 
  // not much other times to do it and it will take a short delay. If we do it each 
  // step then it creates a visual effect on the LEDs
  SerialComs::checkSerial();
}
