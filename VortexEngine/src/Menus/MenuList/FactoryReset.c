#include "FactoryReset.h"
#include <stdlib.h>

#include "../../VortexEngine.h"
#include "../../Modes/DefaultModes.h"
#include "../../Time/TimeControl.h"
#include "../../Patterns/Pattern.h"
#include "../../Buttons/Button.h"
#include "../../Time/Timings.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include "../../VortexConfig.h"

#define COL32(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })

static void showReset(FactoryResetMenu *self);

extern void VortexEngine_setAutoCycle(bool enabled);

Menu *FactoryResetMenu_Create(RGBColor col, bool advanced)
{
  FactoryResetMenu *self = (FactoryResetMenu *)malloc(sizeof(FactoryResetMenu));
  if (!self) return NULL;
  Menu_construct(&self->base, col, advanced);
  self->base.vtable = &g_factoryResetMenuVTable;
  return &self->base;
}

void FactoryResetMenu_destroy(Menu *self)
{
  (void)self;
}

bool FactoryResetMenu_init(Menu *self)
{
  if (!Menu_init(self)) {
    return false;
  }
  Mode *cur = Modes_curMode();
  if (!cur) {
    return false;
  }
  if (Mode_isMultiLed(cur)) {
    self->ledSelected = true;
    self->targetLeds = MAP_LED(LED_MULTI);
  }
  if (!self->advanced) {
    self->ledSelected = true;
  }
  self->curSelection = 0;
  DEBUG_LOG("Entered factory reset");
  return true;
}

MenuAction FactoryResetMenu_run(Menu *self)
{
  MenuAction result = Menu_run(self);
  if (result != MENU_CONTINUE) {
    return result;
  }
  showReset((FactoryResetMenu *)self);
  return MENU_CONTINUE;
}

void FactoryResetMenu_onShortClick(Menu *self)
{
  self->curSelection = (uint8_t)!self->curSelection;
}

void FactoryResetMenu_onLongClick(Menu *self)
{
  if (self->curSelection == 0) {
    Menu_leaveMenu(self, false);
    return;
  }
  if (Button_holdDuration(g_pButton) <= (FACTORY_RESET_THRESHOLD_TICKS + MS_TO_TICKS(10))) {
    return;
  }
  if (self->advanced) {
    const struct DefaultModeEntry *defMode = &defaultModes[Modes_curModeIndex()];
    Mode *cur = Modes_curMode();
    MAP_FOREACH_LED(self->targetLeds) {
      const struct DefaultLedEntry *led = &defMode->leds[pos];
      Colorset set;
      Colorset_initFromU32s(&set, led->numColors, led->cols);
      Mode_setPattern(cur, led->patternID, pos, NULL, &set);
    }
    Mode_init(cur);
  } else {
    Leds_setBrightness(DEFAULT_BRIGHTNESS);
    VortexEngine_setAutoCycle(false);
    Modes_setDefaults();
    Modes_setCurMode(0);
    Modes_resetFlags();
  }
  Menu_leaveMenu(self, true);
}

static void showReset(FactoryResetMenu *self)
{
  (void)self;
  if (self->base.curSelection == 0) {
    Leds_clearAll();
    Leds_blinkAll(350, 350, COL32(RGB_WHITE0));
    return;
  }
  bool isPressed = Button_isPressed(g_pButton);
  if (!isPressed) {
    Leds_clearAll();
    Leds_blinkAll(50, 50, COL32(RGB_RED4));
    return;
  }
  uint32_t holdDur = Button_holdDuration(g_pButton);
  if (holdDur < MS_TO_TICKS(100)) {
    return;
  }
  uint16_t progress = ((holdDur * 100) / FACTORY_RESET_THRESHOLD_TICKS);
  DEBUG_LOGF("progress: %d", progress);
  if (progress >= 100) {
    Leds_setAll(COL32(RGB_WHITE));
    return;
  }
  uint8_t offMs = 100;
  uint8_t onMs = (progress > 60) ? 30 : 100;
  uint8_t sat = (uint8_t)((progress * 5) >> 1);
  Leds_clearAll();
  HSVColor hsv;
  HSVColor_initHSV(&hsv, 0, 255 - sat, 180);
  RGBColor rgb;
  RGBColor_initFromHSV(&rgb, &hsv);
  Leds_blinkIndex(LED_0, offMs, onMs, rgb);
  Leds_blinkIndex(LED_1, offMs, onMs, COL32(RGB_WHITE0));
}

const MenuVTable g_factoryResetMenuVTable = {
  .destroy = FactoryResetMenu_destroy,
  .init = FactoryResetMenu_init,
  .run = FactoryResetMenu_run,
  .onLedSelected = Menu_onLedSelected,
  .onShortClick = FactoryResetMenu_onShortClick,
  .onLongClick = FactoryResetMenu_onLongClick,
  .leaveMenu = Menu_leaveMenu,
};
