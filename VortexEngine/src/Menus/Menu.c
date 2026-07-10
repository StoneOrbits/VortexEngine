#include "Menu.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Buttons/Button.h"
#include "../Menus/Menus.h"
#include "../Modes/Modes.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

void Menu_construct(Menu *self, RGBColor col, bool advanced)
{
  self->vtable = &g_menuVTable;
  Mode_init(&self->previewMode);
  self->menuColor = col;
  self->targetLeds = MAP_LED_ALL;
  self->curSelection = 0;
  self->ledSelected = false;
  self->advanced = advanced;
  self->shouldClose = false;
}

void Menu_destroy(Menu *self)
{
  (void)self;
}

bool Menu_init(Menu *self)
{
  if (!Modes_curMode()) {
    if (Modes_numModes() > 0) {
      return false;
    }
    {
      Mode tmpMode;
      Mode_initFromID(&tmpMode, PATTERN_STROBE, NULL);
      if (!Modes_addModeMode(&tmpMode)) {
        return false;
      }
    }
    if (!Modes_curMode()) {
      return false;
    }
  }
  Mode_assign(&self->previewMode, Modes_curMode());
  Mode_init(&self->previewMode);
  self->shouldClose = false;
  return true;
}

MenuAction Menu_run(Menu *self)
{
  if (self->shouldClose) {
    self->shouldClose = false;
    return MENU_QUIT;
  }

  if (self->ledSelected) {
    return MENU_CONTINUE;
  }

  if (Button_onShortClick(g_pButton)) {
    Menu_nextBulbSelection(self);
  }
  if (Button_onLongClick(g_pButton)) {
    self->ledSelected = true;
    self->vtable->onLedSelected(self);
    DEBUG_LOGF("Led Selected: 0x%x (%s)", self->targetLeds,
      (self->targetLeds == MAP_LED(LED_MULTI)) ? "multi"
        : (self->targetLeds == MAP_LED_ALL) ? "all"
          : "some singles");
  }

  Menu_showBulbSelection(self);

  return MENU_SKIP;
}

void Menu_showBulbSelection(Menu *self)
{
  (void)self;
  Leds_clearAll();
  Leds_blinkMap(self->targetLeds, BULB_SELECT_OFF_MS, BULB_SELECT_ON_MS, RGB_COLOR(RGB_MAGENTA1));
  Menus_showSelection(RGB_COLOR(RGB_MAGENTA1));
}

void Menu_showExit(Menu *self)
{
  (void)self;
  if (Button_isPressed(g_pButton) && Button_holdDuration(g_pButton) > SHORT_CLICK_THRESHOLD_TICKS) {
    Leds_setIndex(LED_1, RGB_COLOR(RGB_RED));
  } else {
    Leds_clearIndex(LED_1);
    Leds_blinkIndex(LED_0, EXIT_MENU_OFF_MS, EXIT_MENU_ON_MS, RGB_COLOR(RGB_WHITE0));
    Leds_blinkIndex(LED_1, EXIT_MENU_OFF_MS, EXIT_MENU_ON_MS, RGB_COLOR(RGB_RED0));
  }
}

void Menu_nextBulbSelection(Menu *self)
{
  switch (self->targetLeds) {
  case MAP_LED_ALL:
    self->targetLeds = MAP_LED(LED_FIRST);
    break;
  case MAP_LED(LED_LAST):
    self->targetLeds = MAP_LED_ALL;
    break;
  default:
    self->targetLeds = MAP_LED(((ledmapGetFirstLed(self->targetLeds) + 1) % (LED_COUNT + 1)));
    break;
  }
}

void Menu_bypassLedSelection(Menu *self, LedMap map)
{
  self->ledSelected = true;
  self->targetLeds = map;
  self->vtable->onLedSelected(self);
}

void Menu_onLedSelected(Menu *self)
{
  (void)self;
}

void Menu_onShortClick(Menu *self)
{
  (void)self;
}

void Menu_onLongClick(Menu *self)
{
  Menu_leaveMenu(self, false);
}

void Menu_leaveMenu(Menu *self, bool doSave)
{
  self->shouldClose = true;
  if (doSave) {
    Modes_saveStorage();
  }
}

const MenuVTable g_menuVTable = {
  .destroy = Menu_destroy,
  .init = Menu_init,
  .run = Menu_run,
  .onLedSelected = Menu_onLedSelected,
  .onShortClick = Menu_onShortClick,
  .onLongClick = Menu_onLongClick,
  .leaveMenu = Menu_leaveMenu,
};
