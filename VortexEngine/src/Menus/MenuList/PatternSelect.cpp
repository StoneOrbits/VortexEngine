#include "PatternSelect.h"
#include "../Menus.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/PatternArgs.h"
#include "../../Patterns/Pattern.h"
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Buttons/Button.h"
#include "../../Random/Random.h"
#include "../../Time/Timings.h"
#include "../../Modes/Modes.h"
#include "../../Menus/Menus.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"

// the derived hooks, dispatched through the base Menu vtable
static void PatternSelect_onLedSelectedHook(void *self);
static void PatternSelect_destroyHook(void *self);
static bool PatternSelect_initHook(void *self);
static MenuAction PatternSelect_runHook(void *self);
static void PatternSelect_onShortClickHook(void *self);
static void PatternSelect_onLongClickHook(void *self);

const MenuVtable PatternSelect_vtable = {
  PatternSelect_initHook,
  PatternSelect_runHook,
  PatternSelect_onLedSelectedHook,
  PatternSelect_onShortClickHook,
  PatternSelect_onLongClickHook,
  NULL,
  PatternSelect_destroyHook,
};

PatternSelect *PatternSelect_new(const RGBColor *col, bool advanced)
{
  PatternSelect *self = (PatternSelect *)Menus_allocMenu();
  if (!self) {
    return NULL;
  }
  Menu_initBase(&self->m_base, &PatternSelect_vtable);
  RGBColor_setRGB(&self->m_base.m_menuColor, col);
  self->m_base.m_advanced = advanced;
  self->m_srcLed = LED_FIRST;
  self->m_started = false;
  return self;
}

void PatternSelect_destroy(PatternSelect *self)
{
  Menu_baseDestroy(&self->m_base);
}

bool PatternSelect_init(PatternSelect *self)
{
  if (!Menu_baseInit(&self->m_base)) {
    return false;
  }
  DEBUG_LOG("Entered pattern select");
  return true;
}

MenuAction PatternSelect_run(PatternSelect *self)
{
  Menu *menu = &self->m_base;
  MenuAction result = Menu_runBase(menu);
  if (result != MENU_CONTINUE) {
    return result;
  }
  // run the current mode
  Mode_play(&menu->m_previewMode);
  // show dimmer selections in advanced mode
  Menus_showSelection(RGB_WHITE5);
  return MENU_CONTINUE;
}

void PatternSelect_onLedSelected(PatternSelect *self)
{
  self->m_srcLed = ledmapGetFirstLed(self->m_base.m_targetLeds);
}

void PatternSelect_onShortClick(PatternSelect *self)
{
  Menu *menu = &self->m_base;
  PatternID newID = (PatternID)(Mode_getPatternID(&menu->m_previewMode, self->m_srcLed) + 1);
  if (newID > PATTERN_SINGLE_LAST) {
    newID = PATTERN_SINGLE_FIRST;
    Leds_holdAll(RGB_WHITE);
  }
  if (!self->m_started) {
    self->m_started = true;
    newID = PATTERN_FIRST;
  }
  // set the new pattern id
  if (isMultiLedPatternID(newID)) {
    Mode_setPattern(&menu->m_previewMode, newID, LED_ANY, NULL, NULL);
  } else {
    Mode_setPatternMap(&menu->m_previewMode, menu->m_targetLeds, newID, NULL, NULL);
  }
  Mode_initPatterns(&menu->m_previewMode);
  DEBUG_LOGF("Iterated to pattern id %d", newID);
}

void PatternSelect_onLongClick(PatternSelect *self)
{
  // store the mode as current mode
  Modes_updateCurMode(&self->m_base.m_previewMode);
  Menu_leaveMenu(&self->m_base, true);
}

static void PatternSelect_onLedSelectedHook(void *self)
{
  PatternSelect_onLedSelected((PatternSelect *)self);
}

static void PatternSelect_onShortClickHook(void *self)
{
  PatternSelect_onShortClick((PatternSelect *)self);
}

static void PatternSelect_onLongClickHook(void *self)
{
  PatternSelect_onLongClick((PatternSelect *)self);
}

static bool PatternSelect_initHook(void *self)
{
  return PatternSelect_init((PatternSelect *)self);
}

static MenuAction PatternSelect_runHook(void *self)
{
  return PatternSelect_run((PatternSelect *)self);
}

static void PatternSelect_destroyHook(void *self)
{
  PatternSelect_destroy((PatternSelect *)self);
}
