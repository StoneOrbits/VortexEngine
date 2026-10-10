#include "ModeSharing.h"
#include "../Menus.h"

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
#include "../../Memory/Memory.h"

static void ModeSharing_onLedSelectedHook(void *self);
static void ModeSharing_destroyHook(void *self);
static bool ModeSharing_initHook(void *self);
static MenuAction ModeSharing_runHook(void *self);
static void ModeSharing_onShortClickHook(void *self);
static void ModeSharing_onLongClickHook(void *self);

const MenuVtable ModeSharing_vtable = {
  ModeSharing_initHook,
  ModeSharing_runHook,
  ModeSharing_onLedSelectedHook,
  ModeSharing_onShortClickHook,
  ModeSharing_onLongClickHook,
  NULL,
  ModeSharing_destroyHook,
};

ModeSharing *ModeSharing_new(const RGBColor *col, bool advanced)
{
  ModeSharing *self = (ModeSharing *)Menus_allocMenu();
  if (!self) {
    return NULL;
  }
  Menu_initBase(&self->m_base, &ModeSharing_vtable);
  RGBColor_setRGB(&self->m_base.m_menuColor, col);
  self->m_base.m_advanced = advanced;
  self->m_sharingMode = SHARE_SEND_RECEIVE;
  self->m_timeOutStartTime = 0;
  self->m_lastPercentChange = 0;
  self->m_lastPercent = 0;
  return self;
}

void ModeSharing_destroy(ModeSharing *self)
{
  VLReceiver_endReceiving();
  VortexEngine_toggleForceSleep(true);
  Menu_baseDestroy(&self->m_base);
}

bool ModeSharing_init(ModeSharing *self)
{
  Menu *menu = &self->m_base;
  if (!Menu_baseInit(menu)) {
    return false;
  }
  // skip led selection
  if (!menu->m_advanced) {
    // skip led selection
    menu->m_ledSelected = true;
  }
  // start on receive because it's the more responsive of the two
  // the odds of opening receive and then accidentally receiving
  // a mode that is being broadcast nearby is completely unlikely
  VLReceiver_beginReceiving();
  // turn off force sleep while using modesharing
  VortexEngine_toggleForceSleep(false);
  DEBUG_LOG("Entering Mode Sharing");
  return true;
}

MenuAction ModeSharing_run(ModeSharing *self)
{
  Menu *menu = &self->m_base;
  MenuAction result = Menu_runBase(menu);
  if (result != MENU_CONTINUE) {
    return result;
  }
  if (self->m_sharingMode == SHARE_EXIT) {
    Menu_showExit(menu);
    return MENU_CONTINUE;
  }
  // if the button is held for at least a long click, then continuously send
  if (Button_isPressed(g_pButton) && Button_holdDuration(g_pButton) >= CLICK_THRESHOLD) {
    // exit is handled above so the only two options are normal or legacy
    if (self->m_sharingMode == SHARE_SEND_RECEIVE) {
      // send normal
      VLSender_send(&menu->m_previewMode);
    } else {
      // send legacy
      VLSender_sendLegacy(&menu->m_previewMode);
    }
  }
  // render the 'receive mode' lights whether legacy or not
  ModeSharing_showReceiveMode(self);
  ModeSharing_receiveMode(self);
  return MENU_CONTINUE;
}

void ModeSharing_onLedSelected(ModeSharing *self)
{
  Menu *menu = &self->m_base;
  // if we selected leds that implies advanced mode
  if (menu->m_targetLeds == MAP_LED(LED_1)) {
    // if we selected the top led then simply swap the two patterns
    // so that the top led is sent first -- if the receiver is receiving
    // into one slot then it will only use the first pattern to do so
    Mode_swapPatterns(&menu->m_previewMode, LED_0, LED_1);
  }
}

// handlers for clicks
void ModeSharing_onShortClick(ModeSharing *self)
{
  if (Button_holdDuration(g_pButton) >= CLICK_THRESHOLD) {
    return;
  }
  switch (self->m_sharingMode) {
  case SHARE_SEND_RECEIVE:
    // stop receiving, send the mode, go back to receiving
    VLReceiver_setLegacyReceiver(true);
    self->m_sharingMode = SHARE_SEND_RECEIVE_LEGACY;
    break;
  case SHARE_SEND_RECEIVE_LEGACY:
    VLReceiver_setLegacyReceiver(false);
    self->m_sharingMode = SHARE_EXIT;
    break;
  case SHARE_EXIT:
    self->m_sharingMode = SHARE_SEND_RECEIVE;
    break;
  default:
    break;
  }
  Leds_clearAll();
}

void ModeSharing_onLongClick(ModeSharing *self)
{
  if (self->m_sharingMode == SHARE_EXIT) {
    Menu_leaveMenu(&self->m_base, false);
  }
}

void ModeSharing_receiveMode(ModeSharing *self)
{
  Menu *menu = &self->m_base;
  uint32_t now = Time_getCurtime();
  // if reveiving new data set our last data time
  if (VLReceiver_onNewData()) {
    self->m_timeOutStartTime = now;
    // if our last data was more than time out duration reset the recveiver
  } else if (self->m_timeOutStartTime > 0 && (self->m_timeOutStartTime + MAX_TIMEOUT_DURATION) < now) {
    VLReceiver_resetVLState();
    self->m_timeOutStartTime = 0;
    return;
  }
  // check if the VLReceiver has a full packet available
  if (!VLReceiver_dataReady()) {
    uint8_t percent = VLReceiver_percentReceived();
    if (percent != self->m_lastPercent) {
      self->m_lastPercent = percent;
      self->m_lastPercentChange = now;
    }
    // nothing available yet
    return;
  }
  DEBUG_LOG("Mode ready to receive! Receiving...");
  // receive the VL mode into the current mode
  if (!VLReceiver_receiveMode(&menu->m_previewMode)) {
    ERROR_LOG("Failed to receive mode");
    return;
  }
  DEBUG_LOGF("Success receiving mode: %u", Mode_getPatternID(&menu->m_previewMode, LED_FIRST));
  if (menu->m_advanced && menu->m_targetLeds != MAP_LED_ALL) {
    LedPos target = ledmapGetFirstLed(menu->m_targetLeds);
    LedPos other = LED_1;
    // if the user picked the top led to copy into then swap the patterns
    // in the incoming mode so the 0th pattern is on the top led
    if (target == LED_1) {
      other = LED_0;
      Mode_swapPatterns(&menu->m_previewMode, LED_0, LED_1);
    }
    Mode_copyPatternFrom(&menu->m_previewMode, Modes_curMode(), other, other);
  }
  Modes_updateCurMode(&menu->m_previewMode);
  // leave menu and save settings, even if the mode was the same whatever
  Menu_leaveMenu(menu, true);
}

void ModeSharing_showReceiveMode(ModeSharing *self)
{
  // if the receiver is actively receiving right now then
  if (VLReceiver_isReceiving()) {
    uint32_t diff = (Time_getCurtime() - self->m_lastPercentChange);
    // this generates the red flash when the receiver hasn't received something for
    // some amount of time, 100 is just arbitray idk if it could be a better value
    if (diff > 100) {
      Leds_setIndex(LED_0, RGB_RED3);
    } else {
      Leds_setIndex(LED_0, RGBColor_makeRGB(0, VLReceiver_percentReceived(), 0));
    }
    Leds_clearIndex(LED_1);
  } else {
    Leds_setAll((self->m_sharingMode == SHARE_SEND_RECEIVE) ? RGBColor_fromU32(0x000F05) : RGB_WHITE0);
  }
}

static void ModeSharing_onLedSelectedHook(void *self)
{
  ModeSharing_onLedSelected((ModeSharing *)self);
}

static void ModeSharing_onShortClickHook(void *self)
{
  ModeSharing_onShortClick((ModeSharing *)self);
}

static void ModeSharing_onLongClickHook(void *self)
{
  ModeSharing_onLongClick((ModeSharing *)self);
}

static bool ModeSharing_initHook(void *self)
{
  return ModeSharing_init((ModeSharing *)self);
}

static MenuAction ModeSharing_runHook(void *self)
{
  return ModeSharing_run((ModeSharing *)self);
}

static void ModeSharing_destroyHook(void *self)
{
  ModeSharing_destroy((ModeSharing *)self);
}
