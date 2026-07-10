#include "PatternSelect.h"
#include <stdlib.h>

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

#define COL32(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })

Menu *PatternSelectMenu_Create(RGBColor col, bool advanced)
{
  PatternSelectMenu *self = (PatternSelectMenu *)malloc(sizeof(PatternSelectMenu));
  if (!self) return NULL;
  Menu_construct(&self->base, col, advanced);
  self->base.vtable = &g_patternSelectMenuVTable;
  self->srcLed = LED_FIRST;
  self->started = false;
  return &self->base;
}

void PatternSelectMenu_destroy(Menu *self)
{
  (void)self;
}

bool PatternSelectMenu_init(Menu *self)
{
  if (!Menu_init(self)) {
    return false;
  }
  DEBUG_LOG("Entered pattern select");
  return true;
}

MenuAction PatternSelectMenu_run(Menu *self)
{
  MenuAction result = Menu_run(self);
  if (result != MENU_CONTINUE) {
    return result;
  }
  Mode_play(&self->previewMode);
  Menus_showSelection(COL32(RGB_WHITE5));
  return MENU_CONTINUE;
}

void PatternSelectMenu_onLedSelected(Menu *self)
{
  PatternSelectMenu *ps = (PatternSelectMenu *)self;
  ps->srcLed = ledmapGetFirstLed(self->targetLeds);
}

void PatternSelectMenu_onShortClick(Menu *self)
{
  PatternSelectMenu *ps = (PatternSelectMenu *)self;
  PatternID newID = (PatternID)(Mode_getPatternID(&self->previewMode, ps->srcLed) + 1);
  if (newID > PATTERN_SINGLE_LAST) {
    newID = PATTERN_SINGLE_FIRST;
    Leds_holdAll(COL32(RGB_WHITE));
  }
  if (!ps->started) {
    ps->started = true;
    newID = PATTERN_FIRST;
  }
  if (isMultiLedPatternID(newID)) {
    Mode_setPattern(&self->previewMode, newID, LED_ALL, NULL, NULL);
  } else {
    Mode_setPatternMap(&self->previewMode, self->targetLeds, newID, NULL, NULL);
  }
  Mode_init(&self->previewMode);
  DEBUG_LOGF("Iterated to pattern id %d", newID);
}

void PatternSelectMenu_onLongClick(Menu *self)
{
  Modes_updateCurMode(&self->previewMode);
  Menu_leaveMenu(self, true);
}

const MenuVTable g_patternSelectMenuVTable = {
  .destroy = PatternSelectMenu_destroy,
  .init = PatternSelectMenu_init,
  .run = PatternSelectMenu_run,
  .onLedSelected = PatternSelectMenu_onLedSelected,
  .onShortClick = PatternSelectMenu_onShortClick,
  .onLongClick = PatternSelectMenu_onLongClick,
  .leaveMenu = Menu_leaveMenu,
};
