#include "Menus.h"

// menus
#include "MenuList/GlobalBrightness.h"
#include "MenuList/EditorConnection.h"
#include "MenuList/FactoryReset.h"
#include "MenuList/ModeSharing.h"
#include "MenuList/ColorSelect.h"
#include "MenuList/PatternSelect.h"
#include "MenuList/Randomizer.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../VortexEngine.h"
#include "../Buttons/Button.h"
#include "../Serial/Serial.h"
#include "../Modes/Modes.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"
#include "../Memory/Memory.h"

// static members
MenuState Menus_m_menuState = MENU_STATE_NOT_OPEN;
uint8_t Menus_m_selection = 0;
uint32_t Menus_m_openTime = 0;
Menu *Menus_m_pCurMenu = NULL;

// every menu type, the menu pool blocks are sized to fit the largest
typedef union AnyMenu {
  Menu menu;
  Randomizer randomizer;
  ModeSharing modeSharing;
#if ENABLE_EDITOR_CONNECTION == 1
  EditorConnection editorConnection;
#endif
  ColorSelect colorSelect;
  PatternSelect patternSelect;
  GlobalBrightness globalBrightness;
  FactoryReset factoryReset;
} AnyMenu;

// the pool all menus are allocated from
static MemPool s_menuPool;
static MEMPOOL_STORAGE(s_menuStorage, sizeof(AnyMenu), MENU_POOL_SIZE);

void *Menus_allocMenu()
{
  return MemPool_alloc(&s_menuPool);
}

void Menus_freeMenu(Menu *menu)
{
  MemPool_free(&s_menuPool, menu);
}

// typedef for the menu initialization function
typedef Menu *(*initMenuFn_t)(const RGBColor *col, bool advanced);

// entries for the ring menu
struct MenuEntry {
#if LOGGING_LEVEL > 2
  // only store the name if the logging is enabled
  const char *menuName;
#endif
  initMenuFn_t initMenu;
  RGBColor color;
};

// a factory macro to forward to each derived menu constructor
#define MENU_NEW_FACTORY(name) \
  static Menu *name##_menuNew(const RGBColor *col, bool advanced) \
  { \
    return (Menu *)name##_new(col, advanced); \
  }

MENU_NEW_FACTORY(Randomizer)
MENU_NEW_FACTORY(ModeSharing)
#if ENABLE_EDITOR_CONNECTION == 1
MENU_NEW_FACTORY(EditorConnection)
#endif
MENU_NEW_FACTORY(ColorSelect)
MENU_NEW_FACTORY(PatternSelect)
MENU_NEW_FACTORY(GlobalBrightness)
MENU_NEW_FACTORY(FactoryReset)

// a simple macro to simplify the entries in the menu list
#if LOGGING_LEVEL > 2
// if the logging is enabled then we need to store the name of the menu
#define ENTRY(classname, color) { #classname, classname##_menuNew, color }
#else
#define ENTRY(classname, color) { classname##_menuNew, color }
#endif

// The list of menus that are registered with colors to show in ring menu
const MenuEntry menuList[] = {
  // =========================
  //  Default menu setup:
  ENTRY(Randomizer, RGB_MENU_RANDOMIZER),
  ENTRY(ModeSharing, RGB_MENU_MODE_SHARING),
#if ENABLE_EDITOR_CONNECTION == 1
  ENTRY(EditorConnection, RGB_MENU_EDITOR_CONNECTION),
#endif
  ENTRY(ColorSelect, RGB_MENU_COLOR_SELECT),
  ENTRY(PatternSelect, RGB_MENU_PATTERN_SELECT),
  ENTRY(GlobalBrightness, RGB_MENU_BRIGHTNESS_SELECT),
  ENTRY(FactoryReset, RGB_MENU_FACTORY_RESET),
};

// the number of menus in the above array
#define NUM_MENUS (sizeof(menuList) / sizeof(menuList[0]))

bool Menus_init()
{
  MemPool_init(&s_menuPool, s_menuStorage, sizeof(AnyMenu), MENU_POOL_SIZE);
  MEMPOOL_TRACK(&s_menuPool, "menu", DEVICE_MENU_POOL_SIZE);
  Menus_m_menuState = MENU_STATE_NOT_OPEN;
  // sub-menus are initialized right before they run
  return true;
}

void Menus_cleanup()
{
  if (Menus_m_pCurMenu) {
    Menu_destroy(Menus_m_pCurMenu);
    Menus_m_pCurMenu = NULL;
  }
  Menus_m_menuState = MENU_STATE_NOT_OPEN;
  Menus_m_selection = 0;
  Menus_m_openTime = 0;
}

bool Menus_run()
{
  // if there are no menus, then we don't need to run
  switch (Menus_m_menuState) {
  case MENU_STATE_NOT_OPEN:
  default:
    // nothing to run
    return false;
  case MENU_STATE_MENU_SELECTION:
    return Menus_runMenuSelection();
  case MENU_STATE_IN_MENU:
    return Menus_runCurMenu();
  }
}

bool Menus_runMenuSelection()
{
  if (Button_onShortClick(g_pButton)) {
    // otherwise increment selection and wrap around at num menus
    Menus_m_selection = (Menus_m_selection + 1) % NUM_MENUS;
#if ENABLE_EDITOR_CONNECTION == 1
    // Hide the editor connection menu because it opens automatically
    // TODO: Create a better way to hide this menu color, this menu
    //       will automatically open when the device is plugged in
    if (Menus_m_selection == MENU_EDITOR_CONNECTION) {
      Menus_m_selection++;
    }
#endif
    DEBUG_LOGF("Cyling to ring menu %u", Menus_m_selection);
    // reset the open time so that it starts again
    Menus_m_openTime = Time_getCurtime();
    // clear the leds
    Leds_clearAll();
    return true;
  }
  // clear the leds so it always fills instead of replacing
  Leds_clearAll();
  // timings for blink later
  uint8_t offtime = 200;
  uint8_t ontime = 200;
  // whether advanced menus are enabled
  bool advMenus = Modes_advancedMenusEnabled();
  // if the button was long pressed then select this menu, but we
  // need to check the presstime to ensure we don't catch the initial
  // release after opening the ringmenu
  if (Button_pressTime(g_pButton) >= Menus_m_openTime) {
    // whether to open advanced menus or not
    bool openAdv = (Button_holdDuration(g_pButton) > ADV_MENU_DURATION_TICKS) && advMenus;
    if (Button_onLongClick(g_pButton)) {
      // ringmenu is open so select the menu
      DEBUG_LOGF("Selected ringmenu %s", menuList[Menus_m_selection].menuName);
      // open the menu we have selected
      if (!Menus_openMenu(Menus_m_selection, openAdv)) {
        DEBUG_LOGF("Failed to initialize %s menu", menuList[Menus_m_selection].menuName);
        return false;
      }
      // display the newly opened menu
      return true;
    }
    // if holding down to select the menu option
    if (Button_isPressed(g_pButton) && openAdv) {
      // make it strobe aw yiss
      offtime = HYPERSTROBE_OFF_DURATION;
      ontime = HYPERSTROBE_ON_DURATION;
    }
  }
  // blink every even/odd of every pair
  for (Pair p = PAIR_FIRST; p < PAIR_COUNT; ++p) {
    if (pairEven(p) < LED_COUNT) {
      Leds_blinkIndex(pairEven(p), offtime, ontime, menuList[Menus_m_selection].color);
    }
    if (pairOdd(p) < LED_COUNT) {
      Leds_setIndex(pairOdd(p), menuList[Menus_m_selection].color);
      Leds_blinkIndex(pairOdd(p), offtime, ontime, RGB_OFF);
    }
  }
  // check if the advanced menus have been enabled
  if (Button_onConsecutivePresses(g_pButton, ADVANCED_MENU_CLICKS)) {
    // toggle the advanced menu
    Modes_setAdvancedMenus(!advMenus, true);
    // display a pink or red depending on whether the menu was enabled
    for (uint16_t i = 0; i < 2; ++i) {
      Leds_holdAll(advMenus ? RGB_RED : RGB_PINK);
    }
  }
  // show when the user selects a menu option
  Menus_showSelection(RGBColor_makeRGB(menuList[Menus_m_selection].color.red << 3,
    menuList[Menus_m_selection].color.green << 3,
    menuList[Menus_m_selection].color.blue << 3));
  // continue in the menu
  return true;
}

bool Menus_runCurMenu()
{
  // if the menu run handler returns false that signals the
  // menu was closed by the user leaving the menu
  switch (Menu_run(Menus_m_pCurMenu)) {
  case MENU_QUIT:
    // close the current menu when run returns false
    Menus_closeCurMenu();
    // return false to let the modes play
    return false;
  case MENU_CONTINUE:
    // if Menu continue run the click handlers for the menu
    if (Button_onShortClick(g_pButton)) {
      Menu_onShortClick(Menus_m_pCurMenu);
    }
    if (Button_onLongClick(g_pButton)) {
      Menu_onLongClick(Menus_m_pCurMenu);
    }
    break;
  case MENU_SKIP:
    // dont run click handlers in this case
    // so the core menu class can handle them
    break;
  }
  // the opened menu and don't play modes
  return true;
}

// open the menu selection ring
bool Menus_openMenuSelection()
{
  if (Menus_m_menuState != MENU_STATE_NOT_OPEN) {
    return false;
  }
  // save the time of when we open the menu so we can fill based on curtime from then
  Menus_m_openTime = Time_getCurtime();
  // open the menu
  Menus_m_menuState = MENU_STATE_MENU_SELECTION;
  // clear the leds
  Leds_clearAll();
  return true;
}

bool Menus_openMenu(uint32_t index, bool advanced)
{
  if (index >= NUM_MENUS) {
    return false;
  }
  // the menu pool only has room for one menu so the open menu has to be
  // freed before the new one can be allocated
  if (Menus_m_pCurMenu) {
    Menu_destroy(Menus_m_pCurMenu);
    Menus_m_pCurMenu = NULL;
    Menus_m_menuState = MENU_STATE_NOT_OPEN;
  }
  Menus_m_selection = index;
  Menu *newMenu = menuList[Menus_m_selection].initMenu(&menuList[Menus_m_selection].color, advanced);
  if (!newMenu) {
    return false;
  }
  // initialiaze the new menu with the current mode
  if (!Menu_init(newMenu)) {
    DEBUG_LOGF("Failed to initialize %s menu", menuList[Menus_m_selection].menuName);
    // if the menu failed to init, don't open it
    Menu_destroy(newMenu);
    return false;
  }
  // assign the new menu
  Menus_m_pCurMenu = newMenu;
  // and the menus are open just in case
  // clear all the leds
  Leds_clearAll();
  Menus_m_menuState = MENU_STATE_IN_MENU;
  return true;
}

void Menus_showSelection(RGBColor colval)
{
  // blink the tip led white for 150ms when the short
  // click threshold has been surpassed
  if (Button_isPressed(g_pButton) &&
    Button_holdDuration(g_pButton) > SHORT_CLICK_THRESHOLD_TICKS &&
    Button_holdDuration(g_pButton) < (SHORT_CLICK_THRESHOLD_TICKS + MS_TO_TICKS(250))) {
    Leds_setAll(colval);
  }
}

bool Menus_checkOpen()
{
  return Menus_m_menuState != MENU_STATE_NOT_OPEN && Button_releaseTime(g_pButton) > Menus_m_openTime;
}

bool Menus_checkInMenu()
{
  return Menus_m_menuState == MENU_STATE_IN_MENU;
}

Menu *Menus_curMenu()
{
  return Menus_m_pCurMenu;
}

MenuEntryID Menus_curMenuID()
{
  return (MenuEntryID)Menus_m_selection;
}

void Menus_closeCurMenu()
{
  // delete the currently open menu object
  if (Menus_m_pCurMenu) {
    Menu_destroy(Menus_m_pCurMenu);
    Menus_m_pCurMenu = NULL;
  }
  Menus_m_menuState = MENU_STATE_NOT_OPEN;
  Menus_m_selection = 0;
  // clear the leds
  Leds_clearAll();
}
