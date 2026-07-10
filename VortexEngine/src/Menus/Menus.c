#include "Menus.h"

#include "Menu.h"
#include "MenuList/GlobalBrightness.h"
#include "MenuList/EditorConnection.h"
#include "MenuList/FactoryReset.h"
#include "MenuList/ModeSharing.h"
#include "MenuList/ColorSelect.h"
#include "MenuList/PatternSelect.h"
#include "MenuList/Randomizer.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Buttons/Button.h"
#include "../Modes/Modes.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

#define COL32(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })

// Menu state
static enum {
  MENU_STATE_NOT_OPEN = 0,
  MENU_STATE_MENU_SELECTION,
  MENU_STATE_IN_MENU
} m_menuState;

static uint8_t m_selection;
static uint32_t m_openTime;
static Menu *m_pCurMenu;

typedef Menu *(*initMenuFn_t)(RGBColor col, bool advanced);

struct MenuEntry {
#if LOGGING_LEVEL > 2
  const char *menuName;
#endif
  initMenuFn_t initMenu;
  RGBColor color;
};

#if LOGGING_LEVEL > 2
#define ENTRY(init_fn, color) { #init_fn, init_fn, COL32(color) }
#else
#define ENTRY(init_fn, color) { init_fn, COL32(color) }
#endif

const struct MenuEntry menuList[] = {
  ENTRY(RandomizerMenu_Create, RGB_MENU_RANDOMIZER),
  ENTRY(ModeSharingMenu_Create, RGB_MENU_MODE_SHARING),
#if ENABLE_EDITOR_CONNECTION == 1
  ENTRY(EditorConnectionMenu_Create, RGB_MENU_EDITOR_CONNECTION),
#endif
  ENTRY(ColorSelectMenu_Create, RGB_MENU_COLOR_SELECT),
  ENTRY(PatternSelectMenu_Create, RGB_MENU_PATTERN_SELECT),
  ENTRY(GlobalBrightnessMenu_Create, RGB_MENU_BRIGHTNESS_SELECT),
  ENTRY(FactoryResetMenu_Create, RGB_MENU_FACTORY_RESET),
};

#define NUM_MENUS (sizeof(menuList) / sizeof(menuList[0]))

static bool runMenuSelection(void);
static bool runCurMenu(void);
static void closeCurMenu(void);

bool Menus_init(void)
{
  m_menuState = MENU_STATE_NOT_OPEN;
  m_selection = 0;
  m_openTime = 0;
  m_pCurMenu = NULL;
  return true;
}

void Menus_cleanup(void)
{
  if (m_pCurMenu) {
    m_pCurMenu->vtable->destroy(m_pCurMenu);
    m_pCurMenu = NULL;
  }
  m_menuState = MENU_STATE_NOT_OPEN;
  m_selection = 0;
  m_openTime = 0;
}

bool Menus_run(void)
{
  switch (m_menuState) {
  case MENU_STATE_NOT_OPEN:
  default:
    return false;
  case MENU_STATE_MENU_SELECTION:
    return runMenuSelection();
  case MENU_STATE_IN_MENU:
    return runCurMenu();
  }
}

static bool runMenuSelection(void)
{
  if (Button_onShortClick(g_pButton)) {
    m_selection = (m_selection + 1) % NUM_MENUS;
#if ENABLE_EDITOR_CONNECTION == 1
    if (m_selection == MENU_EDITOR_CONNECTION) {
      m_selection++;
    }
#endif
    DEBUG_LOGF("Cyling to ring menu %u", m_selection);
    m_openTime = Time_getCurtime();
    Leds_clearAll();
    return true;
  }
  Leds_clearAll();
  uint8_t offtime = 200;
  uint8_t ontime = 200;
  bool advMenus = Modes_advancedMenusEnabled();
  if (Button_pressTime(g_pButton) >= m_openTime) {
    bool openAdv = (Button_holdDuration(g_pButton) > ADV_MENU_DURATION_TICKS) && advMenus;
    if (Button_onLongClick(g_pButton)) {
      DEBUG_LOGF("Selected ringmenu %s", menuList[m_selection].menuName);
      if (!Menus_openMenu(m_selection, openAdv)) {
        DEBUG_LOGF("Failed to initialize %s menu", menuList[m_selection].menuName);
        return false;
      }
      return true;
    }
    if (Button_isPressed(g_pButton) && openAdv) {
      offtime = HYPERSTROBE_OFF_DURATION;
      ontime = HYPERSTROBE_ON_DURATION;
    }
  }
  for (Pair p = PAIR_FIRST; p < PAIR_COUNT; ++p) {
    if (pairEven(p) < LED_COUNT) {
      Leds_blinkIndex(pairEven(p), offtime, ontime, menuList[m_selection].color);
    }
    if (pairOdd(p) < LED_COUNT) {
      Leds_setIndex(pairOdd(p), menuList[m_selection].color);
      Leds_blinkIndex(pairOdd(p), offtime, ontime, COL32(RGB_OFF));
    }
  }
  if (Button_onConsecutivePresses(g_pButton, ADVANCED_MENU_CLICKS)) {
    Modes_setAdvancedMenus(!advMenus, true);
    for (uint16_t i = 0; i < 2; ++i) {
      Leds_holdAll(advMenus ? COL32(RGB_RED) : COL32(RGB_PINK));
    }
  }
  RGBColor selCol;
  selCol.red = menuList[m_selection].color.red << 3;
  selCol.green = menuList[m_selection].color.green << 3;
  selCol.blue = menuList[m_selection].color.blue << 3;
  Menus_showSelection(selCol);
  return true;
}

static bool runCurMenu(void)
{
  MenuAction action = m_pCurMenu->vtable->run(m_pCurMenu);
  switch (action) {
  case MENU_QUIT:
    closeCurMenu();
    return false;
  case MENU_CONTINUE:
    if (Button_onShortClick(g_pButton)) {
      m_pCurMenu->vtable->onShortClick(m_pCurMenu);
    }
    if (Button_onLongClick(g_pButton)) {
      m_pCurMenu->vtable->onLongClick(m_pCurMenu);
    }
    break;
  case MENU_SKIP:
    break;
  }
  return true;
}

bool Menus_openMenuSelection(void)
{
  if (m_menuState != MENU_STATE_NOT_OPEN) {
    return false;
  }
  m_openTime = Time_getCurtime();
  m_menuState = MENU_STATE_MENU_SELECTION;
  Leds_clearAll();
  return true;
}

bool Menus_openMenu(uint32_t index, bool advanced)
{
  if (index >= NUM_MENUS) {
    return false;
  }
  m_selection = index;
  Menu *newMenu = menuList[m_selection].initMenu(menuList[m_selection].color, advanced);
  if (!newMenu) {
    return false;
  }
  if (!newMenu->vtable->init(newMenu)) {
    DEBUG_LOGF("Failed to initialize %s menu", menuList[m_selection].menuName);
    newMenu->vtable->destroy(newMenu);
    return false;
  }
  if (m_pCurMenu) {
    m_pCurMenu->vtable->destroy(m_pCurMenu);
  }
  m_pCurMenu = newMenu;
  Leds_clearAll();
  m_menuState = MENU_STATE_IN_MENU;
  return true;
}

void Menus_showSelection(RGBColor colval)
{
  if (Button_isPressed(g_pButton) &&
    Button_holdDuration(g_pButton) > SHORT_CLICK_THRESHOLD_TICKS &&
    Button_holdDuration(g_pButton) < (SHORT_CLICK_THRESHOLD_TICKS + MS_TO_TICKS(250))) {
    Leds_setAll(colval);
  }
}

bool Menus_checkOpen(void)
{
  return m_menuState != MENU_STATE_NOT_OPEN && Button_releaseTime(g_pButton) > m_openTime;
}

bool Menus_checkInMenu(void)
{
  return m_menuState == MENU_STATE_IN_MENU;
}

Menu *Menus_curMenu(void)
{
  return m_pCurMenu;
}

enum MenuEntryID Menus_curMenuID(void)
{
  return (enum MenuEntryID)m_selection;
}

static void closeCurMenu(void)
{
  if (m_pCurMenu) {
    m_pCurMenu->vtable->destroy(m_pCurMenu);
    m_pCurMenu = NULL;
  }
  m_menuState = MENU_STATE_NOT_OPEN;
  m_selection = 0;
  Leds_clearAll();
}
