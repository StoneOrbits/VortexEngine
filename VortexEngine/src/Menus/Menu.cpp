#include "Menu.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Buttons/Buttons.h"
#include "../Menus/Menus.h"
#include "../Modes/Modes.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

Menu::Menu(const RGBColor &col, bool advanced) :
  m_previewMode(),
  m_menuColor(col),
  m_targetLeds(MAP_LED_ALL),
  m_curSelection(0),
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
  if (!Modes::curMode()) {
    // if you enter a menu and there's no modes, it will add an empty one
    if (Modes::numModes() > 0) {
      // some kind of serious error
      return false;
    }
    if (!Modes::addMode(PATTERN_STROBE, RGBColor(RGB_OFF))) {
      // some kind of serious error
      return false;
    }
    if (!Modes::curMode()) {
      // serious error again
      return false;
    }
  }
  // copy the current mode into the demo mode and initialize it
  m_previewMode = *Modes::curMode();
  m_previewMode.init();
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
    do {
      nextBulbSelection();
    } while (!isValidLedSelection(m_targetLeds));
  }
  // on a long press of the button, lock in the target led
  if (g_pButton->onLongClick()) {
    m_ledSelected = true;
    // call led selected callback
    onLedSelected();
    // log about the selection
    DEBUG_LOGF("Led Selected: 0x%x (%s)", m_targetLeds,
      (m_targetLeds == MAP_LED(LED_MULTI)) ? "multi"
        : (m_targetLeds == MAP_LED_ALL) ? "all"
          : "some singles");
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
    Leds::blinkIndexOffset(pos, pos * 10, 50, 500, RGB_MAGENTA1);
  } else {
    Leds::blinkMap(m_targetLeds, BULB_SELECT_OFF_MS, BULB_SELECT_ON_MS, RGB_MAGENTA1);
  }
  // blink when selecting
  Menus::showSelection(RGB_MAGENTA1);
}

void Menu::showExit()
{
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    Leds::setAll(RGB_RED);
    return;
  }
  Leds::clearAll();
  Leds::setAll(RGB_WHITE0);
  Leds::blinkAll(EXIT_MENU_OFF_MS, EXIT_MENU_ON_MS, RGB_RED0);
}

// The groups of leds that can be selected before the individual leds, these
// are the same groupings as the original Vortex (Orbit)
static const LedMap bulbSelectionGroups[] = {
  MAP_LED_ALL,
  MAP_LED(LED_MULTI),
  MAP_RINGS_EVEN,
  MAP_RINGS_ODD,
  MAP_RING_EDGE,
  MAP_ALL_TOP,
  MAP_ALL_BOT,
  MAP_LINE_1,
  MAP_LINE_2,
  MAP_QUADRANT_1,
  MAP_QUADRANT_2,
  MAP_QUADRANT_3,
  MAP_QUADRANT_4,
};
#define NUM_BULB_SELECTION_GROUPS (sizeof(bulbSelectionGroups) / sizeof(bulbSelectionGroups[0]))

void Menu::nextBulbSelection()
{
  // cycle through each of the groups first
  for (uint32_t i = 0; i < NUM_BULB_SELECTION_GROUPS; ++i) {
    if (m_targetLeds == bulbSelectionGroups[i]) {
      // after the last group move to the individual leds
      m_targetLeds = (i + 1 < NUM_BULB_SELECTION_GROUPS) ? bulbSelectionGroups[i + 1] : MAP_LED(LED_FIRST);
      return;
    }
  }
  // then each individual led, after the last led wrap back to all
  if (m_targetLeds == MAP_LED(LED_LAST)) {
    m_targetLeds = MAP_LED_ALL;
    return;
  }
  m_targetLeds = MAP_LED(ledmapGetFirstLed(m_targetLeds) + 1);
}

void Menu::bypassLedSelection(LedMap map)
{
  m_ledSelected = true;
  m_targetLeds = map;
  onLedSelected();
}

void Menu::onLedSelected()
{
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
