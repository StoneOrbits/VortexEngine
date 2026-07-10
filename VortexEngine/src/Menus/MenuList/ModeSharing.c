#include "ModeSharing.h"
#include <stdlib.h>

#include "../../VortexEngine.h"
#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Wireless/VLReceiver.h"
#include "../../Wireless/VLSender.h"
#include "../../Patterns/Pattern.h"
#include "../../Buttons/Button.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#define COL32(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })

static void receiveMode(ModeSharingMenu *self);
static void showReceiveMode(ModeSharingMenu *self);

extern void VortexEngine_toggleForceSleep(bool enabled);

Menu *ModeSharingMenu_Create(RGBColor col, bool advanced)
{
  ModeSharingMenu *self = (ModeSharingMenu *)malloc(sizeof(ModeSharingMenu));
  if (!self) return NULL;
  Menu_construct(&self->base, col, advanced);
  self->base.vtable = &g_modeSharingMenuVTable;
  self->sharingMode = SHARE_SEND_RECEIVE;
  self->timeOutStartTime = 0;
  self->lastPercentChange = 0;
  self->lastPercent = 0;
  return &self->base;
}

void ModeSharingMenu_destroy(Menu *self)
{
  (void)self;
  VLReceiver_endReceiving();
  VortexEngine_toggleForceSleep(true);
}

bool ModeSharingMenu_init(Menu *self)
{
  if (!Menu_init(self)) {
    return false;
  }
  ModeSharingMenu *ms = (ModeSharingMenu *)self;
  if (!self->advanced) {
    self->ledSelected = true;
  }
  VLReceiver_beginReceiving();
  VortexEngine_toggleForceSleep(false);
  DEBUG_LOG("Entering Mode Sharing");
  return true;
}

MenuAction ModeSharingMenu_run(Menu *self)
{
  MenuAction result = Menu_run(self);
  if (result != MENU_CONTINUE) {
    return result;
  }
  ModeSharingMenu *ms = (ModeSharingMenu *)self;
  if (ms->sharingMode == SHARE_EXIT) {
    Menu_showExit(self);
    return MENU_CONTINUE;
  }
  if (Button_isPressed(g_pButton) && Button_holdDuration(g_pButton) >= CLICK_THRESHOLD) {
    if (ms->sharingMode == SHARE_SEND_RECEIVE) {
      VLSender_send(&self->previewMode);
    } else {
      VLSender_sendLegacy(&self->previewMode);
    }
  }
  showReceiveMode(ms);
  receiveMode(ms);
  return MENU_CONTINUE;
}

void ModeSharingMenu_onLedSelected(Menu *self)
{
  if (self->targetLeds == MAP_LED(LED_1)) {
    Mode_swapPatterns(&self->previewMode, LED_0, LED_1);
  }
}

void ModeSharingMenu_onShortClick(Menu *self)
{
  if (Button_holdDuration(g_pButton) >= CLICK_THRESHOLD) {
    return;
  }
  ModeSharingMenu *ms = (ModeSharingMenu *)self;
  switch (ms->sharingMode) {
  case SHARE_SEND_RECEIVE:
    VLReceiver_setLegacyReceiver(true);
    ms->sharingMode = SHARE_SEND_RECEIVE_LEGACY;
    break;
  case SHARE_SEND_RECEIVE_LEGACY:
    VLReceiver_setLegacyReceiver(false);
    ms->sharingMode = SHARE_EXIT;
    break;
  case SHARE_EXIT:
    ms->sharingMode = SHARE_SEND_RECEIVE;
    break;
  }
  Leds_clearAll();
}

void ModeSharingMenu_onLongClick(Menu *self)
{
  ModeSharingMenu *ms = (ModeSharingMenu *)self;
  if (ms->sharingMode == SHARE_EXIT) {
    Menu_leaveMenu(self, false);
  }
}

static void receiveMode(ModeSharingMenu *self)
{
  uint32_t now = Time_getCurtime();
  if (VLReceiver_onNewData()) {
    self->timeOutStartTime = now;
  } else if (self->timeOutStartTime > 0 && (self->timeOutStartTime + MAX_TIMEOUT_DURATION) < now) {
    VLReceiver_resetVLState();
    self->timeOutStartTime = 0;
    return;
  }
  if (!VLReceiver_dataReady()) {
    uint8_t percent = VLReceiver_percentReceived();
    if (percent != self->lastPercent) {
      self->lastPercent = percent;
      self->lastPercentChange = now;
    }
    return;
  }
  DEBUG_LOG("Mode ready to receive! Receiving...");
  if (!VLReceiver_receiveMode(&self->base.previewMode)) {
    ERROR_LOG("Failed to receive mode");
    return;
  }
  DEBUG_LOGF("Success receiving mode: %u", Mode_getPatternID(&self->base.previewMode, LED_ALL));
  if (self->base.advanced && self->base.targetLeds != MAP_LED_ALL) {
    LedPos target = ledmapGetFirstLed(self->base.targetLeds);
    LedPos other = LED_1;
    if (target == LED_1) {
      other = LED_0;
      Mode_swapPatterns(&self->base.previewMode, LED_0, LED_1);
    }
    Mode_copyPatternFrom(&self->base.previewMode, Modes_curMode(), other, other);
  }
  Modes_updateCurMode(&self->base.previewMode);
  Menu_leaveMenu(&self->base, true);
}

static void showReceiveMode(ModeSharingMenu *self)
{
  if (VLReceiver_isReceiving()) {
    uint32_t diff = (Time_getCurtime() - self->lastPercentChange);
    if (diff > 100) {
      Leds_setIndex(LED_0, COL32(RGB_RED3));
    } else {
      uint8_t pct = VLReceiver_percentReceived();
      Leds_setIndex(LED_0, (RGBColor){ 0, pct, 0 });
    }
    Leds_clearIndex(LED_1);
  } else {
    Leds_setAll((self->sharingMode == SHARE_SEND_RECEIVE) ? COL32(0x000F05) : COL32(RGB_WHITE0));
  }
}

const MenuVTable g_modeSharingMenuVTable = {
  .destroy = ModeSharingMenu_destroy,
  .init = ModeSharingMenu_init,
  .run = ModeSharingMenu_run,
  .onLedSelected = ModeSharingMenu_onLedSelected,
  .onShortClick = ModeSharingMenu_onShortClick,
  .onLongClick = ModeSharingMenu_onLongClick,
  .leaveMenu = Menu_leaveMenu,
};
