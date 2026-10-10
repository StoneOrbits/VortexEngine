#include "Menu.h"

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Buttons/Button.h"
#include "../Menus/Menus.h"
#include "../Modes/Modes.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"
#include "../Memory/Memory.h"

Menu *Menu_new(const RGBColor *col, bool advanced)
{
  Menu *self = (Menu *)Menus_allocMenu();
  if (!self) {
    return NULL;
  }
  Menu_initBase(self, NULL);
  RGBColor_setRGB(&self->m_menuColor, col);
  self->m_advanced = advanced;
  return self;
}

void Menu_initBase(Menu *self, const MenuVtable *vtable)
{
  Mode_init(&self->m_previewMode);
  self->m_vtable = vtable;
  self->m_menuColor = RGBColor_make();
  self->m_targetLeds = MAP_LED_ALL;
  self->m_curSelection = 0;
  self->m_ledSelected = false;
  self->m_advanced = false;
  self->m_shouldClose = false;
}

void Menu_destroy(Menu *self)
{
  if (!self) {
    return;
  }
  if (self->m_vtable && self->m_vtable->destroy) {
    self->m_vtable->destroy(self);
    return;
  }
  Menu_baseDestroy(self);
}

void Menu_baseDestroy(Menu *self)
{
  Mode_destroy(&self->m_previewMode);
  Menus_freeMenu(self);
}

bool Menu_init(Menu *self)
{
  if (self->m_vtable && self->m_vtable->init) {
    return self->m_vtable->init(self);
  }
  return Menu_baseInit(self);
}

bool Menu_baseInit(Menu *self)
{
  // menu is initialized before being run
  if (!Modes_curMode()) {
    // if you enter a menu and there's no modes, it will add an empty one
    if (Modes_numModes() > 0) {
      // some kind of serious error
      return false;
    }
    if (!Modes_addMode(PATTERN_STROBE, RGBColor_make(), RGBColor_make(), RGBColor_make(),
      RGBColor_make(), RGBColor_make(), RGBColor_make(), RGBColor_make(),
      RGBColor_make())) {
      // some kind of serious error
      return false;
    }
    if (!Modes_curMode()) {
      // serious error again
      return false;
    }
  }
  // copy the current mode into the demo mode and initialize it
  Mode_assign(&self->m_previewMode, Modes_curMode());
  Mode_initPatterns(&self->m_previewMode);
  // just in case
  self->m_shouldClose = false;
  return true;
}

MenuAction Menu_run(Menu *self)
{
  if (self->m_vtable && self->m_vtable->run) {
    return self->m_vtable->run(self);
  }
  return Menu_runBase(self);
}

MenuAction Menu_runBase(Menu *self)
{
  // check whether the close flag has been set
  if (self->m_shouldClose) {
    // reset flag and close
    self->m_shouldClose = false;
    return MENU_QUIT;
  }

  // if the led has already been selected continue as normal
  // and allow derived menu logic to perform work
  if (self->m_ledSelected) {
    return MENU_CONTINUE;
  }

  // using manual onShortClick and onLongClick check here because
  // there is no guarantee the child class will call the parent
  // class's onShortClick and onLongClick functions so

  // every time the button is clicked, change the target led
  if (Button_onShortClick(g_pButton)) {
    Menu_nextBulbSelection(self);
  }
  // on a long press of the button, lock in the target led
  if (Button_onLongClick(g_pButton)) {
    self->m_ledSelected = true;
    // call led selected callback
    Menu_onLedSelected(self);
    // log about the selection
    DEBUG_LOGF("Led Selected: 0x%x (%s)", self->m_targetLeds,
      (self->m_targetLeds == MAP_LED(LED_MULTI)) ? "multi"
        : (self->m_targetLeds == MAP_LED_ALL) ? "all"
          : "some singles");
  }

  // render the bulb selection
  Menu_showBulbSelection(self);

  // return MENU_SKIP to indicate to derived menu classes that
  // they should skip their logic and just return so that the
  // bulb selection can be rendered
  return MENU_SKIP;
}

void Menu_showBulbSelection(Menu *self)
{
  Leds_clearAll();
  Leds_blinkMap(self->m_targetLeds, BULB_SELECT_OFF_MS, BULB_SELECT_ON_MS, RGB_MAGENTA1);
  // blink when selecting
  Menus_showSelection(RGB_MAGENTA1);
}

void Menu_showExit(Menu *self)
{
  if (Button_isPressed(g_pButton) && Button_holdDuration(g_pButton) > SHORT_CLICK_THRESHOLD_TICKS) {
    Leds_setIndex(LED_1, RGB_RED);
  } else {
    Leds_clearIndex(LED_1);
    Leds_blinkIndex(LED_0, EXIT_MENU_OFF_MS, EXIT_MENU_ON_MS, RGB_WHITE0);
    Leds_blinkIndex(LED_1, EXIT_MENU_OFF_MS, EXIT_MENU_ON_MS, RGB_RED0);
  }
}

void Menu_nextBulbSelection(Menu *self)
{
  // The target led can be 0 through LED_COUNT to represent any led or all leds
  // modulo by LED_COUNT + 1 to include LED_COUNT (all) as a target
  switch (self->m_targetLeds) {
  case MAP_LED_ALL:
    self->m_targetLeds = MAP_LED(LED_FIRST);
    break;
  case MAP_LED(LED_LAST):
    self->m_targetLeds = MAP_LED_ALL;
    break;
  default: // LED_FIRST through LED_LAST
    // iterate as normal
    self->m_targetLeds = MAP_LED(((ledmapGetFirstLed(self->m_targetLeds) + 1) % (LED_COUNT + 1)));
    break;
  }
}

void Menu_bypassLedSelection(Menu *self, LedMap map)
{
  self->m_ledSelected = true;
  self->m_targetLeds = map;
  Menu_onLedSelected(self);
}

void Menu_onLedSelected(Menu *self)
{
  if (self->m_vtable && self->m_vtable->onLedSelected) {
    self->m_vtable->onLedSelected(self);
  }
}

void Menu_onShortClick(Menu *self)
{
  if (self->m_vtable && self->m_vtable->onShortClick) {
    self->m_vtable->onShortClick(self);
  }
}

void Menu_onLongClick(Menu *self)
{
  if (self->m_vtable && self->m_vtable->onLongClick) {
    self->m_vtable->onLongClick(self);
    return;
  }
  Menu_leaveMenu(self, false);
}

void Menu_leaveMenu(Menu *self, bool doSave)
{
  if (self->m_vtable && self->m_vtable->leaveMenu) {
    self->m_vtable->leaveMenu(self, doSave);
    return;
  }
  Menu_leaveMenuBase(self, doSave);
}

void Menu_leaveMenuBase(Menu *self, bool doSave)
{
  self->m_shouldClose = true;
  if (doSave) {
    Modes_saveStorage();
  }
}