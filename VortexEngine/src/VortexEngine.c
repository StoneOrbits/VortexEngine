#include "VortexEngine.h"

#include "Wireless/IRReceiver.h"
#include "Wireless/IRSender.h"
#include "Wireless/VLReceiver.h"
#include "Wireless/VLSender.h"
#include "Wireless/IRConfig.h"
#include "Wireless/VLConfig.h"
#include "Storage/Storage.h"
#include "Buttons/Buttons.h"
#include "Time/TimeControl.h"
#include "Time/Timings.h"
#include "Serial/Serial.h"
#include "Modes/Modes.h"
#include "Menus/Menus.h"
#include "Modes/Mode.h"
#include "Leds/Leds.h"

#ifdef VORTEX_EMBEDDED
#include <avr/interrupt.h>
#include <avr/sleep.h>
#endif

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

static volatile bool g_sleeping = false;
static bool g_forceSleepEnabled = true;
static bool g_autoCycle = false;

bool VortexEngine_init(void)
{
#ifdef VORTEX_EMBEDDED
  VortexEngine_clearOutputPins();
#endif

  if (!SerialComs_init()) {
    DEBUG_LOG("Serial failed to initialize");
    return false;
  }
  if (!Time_init()) {
    DEBUG_LOG("Time failed to initialize");
    return false;
  }
  if (!Storage_init()) {
    DEBUG_LOG("Storage failed to initialize");
    return false;
  }
#if IR_ENABLE_RECEIVER == 1
  if (!IRReceiver_init()) {
    DEBUG_LOG("IRReceiver failed to initialize");
    return false;
  }
#endif
#if IR_ENABLE_SENDER == 1
  if (!IRSender_init()) {
    DEBUG_LOG("IRSender failed to initialize");
    return false;
  }
#endif
#if VL_ENABLE_RECEIVER == 1
  if (!VLReceiver_init()) {
    DEBUG_LOG("VLReceiver failed to initialize");
    return false;
  }
#endif
#if VL_ENABLE_SENDER == 1
  if (!VLSender_init()) {
    DEBUG_LOG("VLSender failed to initialize");
    return false;
  }
#endif
  if (!Leds_init()) {
    DEBUG_LOG("Leds failed to initialize");
    return false;
  }
  if (!Buttons_init(0)) {
    DEBUG_LOG("Buttons failed to initialize");
    return false;
  }
  if (!Menus_init()) {
    DEBUG_LOG("Menus failed to initialize");
    return false;
  }
  if (!Modes_init()) {
    DEBUG_LOG("Settings failed to initialize");
    return false;
  }

#if COMPRESSION_TEST == 1
  VortexEngine_compressionTest();
#endif

#if SERIALIZATION_TEST == 1
  VortexEngine_serializationTest();
#endif

#if TIMER_TEST == 1
  VortexEngine_timerTest();
#endif

#ifdef VORTEX_EMBEDDED
  TCB0.CCMP = 10000;
  TCB0.INTCTRL = TCB_CAPT_bm;
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_RUNSTDBY_bm | TCB_ENABLE_bm;
  VortexEngine_enableMOSFET(!Modes_locked());
  set_sleep_mode(SLEEP_MODE_STANDBY);
  sei();
  while (!g_sleeping) {
    sleep_mode();
  }
#endif

  return true;
}

void VortexEngine_cleanup(void)
{
#ifdef VORTEX_LIB
  Modes_cleanup();
  Menus_cleanup();
  Buttons_cleanup();
  Leds_cleanup();
#if VL_ENABLE_SENDER == 1
  VLSender_cleanup();
#endif
#if VL_ENABLE_RECEIVER == 1
  VLReceiver_cleanup();
#endif
#if IR_ENABLE_SENDER == 1
  IRSender_cleanup();
#endif
#if IR_ENABLE_RECEIVER == 1
  IRReceiver_cleanup();
#endif
  Storage_cleanup();
  Time_cleanup();
  SerialComs_cleanup();
#endif
}

void VortexEngine_tick(void)
{
#ifdef VORTEX_LIB
  if (g_sleeping) {
    Buttons_update();
    if (Modes_locked() && Button_onConsecutivePresses(g_pButton, DEVICE_LOCK_CLICKS - 1)) {
      Modes_setLocked(false);
    }
    if (Button_check(g_pButton) || Button_onRelease(g_pButton) || !VortexEngine_isSleeping()) {
      VortexEngine_wakeup();
    }
    return;
  }
#endif

  Time_tickClock();
  Buttons_update();
  VortexEngine_runMainLogic();
  Leds_update();
}

void VortexEngine_runMainLogic(void)
{
  uint32_t now = Time_getCurtime();

  if (Modes_locked()) {
    if (Button_onConsecutivePresses(g_pButton, DEVICE_LOCK_CLICKS - 1)) {
      Modes_setLocked(false, true);
#ifdef VORTEX_EMBEDDED
      VortexEngine_enableMOSFET(true);
#endif
    } else if (now > (CONSECUTIVE_WINDOW_TICKS * DEVICE_LOCK_CLICKS)) {
      VortexEngine_enterSleep(false);
    }
    Leds_clearAll();
    return;
  }

  if (Button_releaseCount(g_pButton) == 0) {
    if (!Modes_load()) {
      return;
    }
    if (!Modes_keychainModeEnabled() && now == ONE_CLICK_THRESHOLD_TICKS && Button_isPressed(g_pButton)) {
      bool isEnabledNow = !Modes_oneClickModeEnabled();
      Modes_setOneClickMode(isEnabledNow, true);
      if (isEnabledNow) {
        Modes_switchToStartupMode();
      } else {
        Modes_setCurMode(0);
      }
      Leds_holdAll(isEnabledNow ? RGB_COLOR(RGB_WHITE0) : RGB_COLOR(RGB_WHITE5));
    }
    return;
  }

#ifdef VORTEX_EMBEDDED
  if (now == 1) {
    if (Modes_getFlag(MODES_FLAG_NEW_FIRMWARE)) {
      Modes_resetFlags();
      Modes_load();
      Modes_saveHeader();
    }
    VortexEngine_enterSleep(false);
    return;
  }
#endif

  if (!Modes_load()) {
    return;
  }

  if (Modes_keychainModeEnabled() && !Menus_checkInMenu()) {
    Modes_switchToStartupMode();
    Menus_openMenu(MENU_GLOBAL_BRIGHTNESS, true);
  }

  const uint32_t holdTime = Button_holdDuration(g_pButton);
  if (g_forceSleepEnabled && holdTime >= FORCE_SLEEP_THRESHOLD_TICKS) {
    if (Button_isPressed(g_pButton)) {
      Leds_clearAll();
      return;
    }
    if (Button_onRelease(g_pButton)) {
      VortexEngine_enterSleep(false);
    }
    return;
  }

  if (Menus_run()) {
    return;
  }

  if (Button_onRelease(g_pButton) && holdTime >= SLEEP_ENTER_THRESHOLD_TICKS) {
    VortexEngine_enterSleep(true);
    return;
  }

  if (Button_isPressed(g_pButton) && holdTime >= SLEEP_ENTER_THRESHOLD_TICKS) {
    Leds_clearAll();
    if (holdTime >= (SLEEP_ENTER_THRESHOLD_TICKS + SLEEP_WINDOW_THRESHOLD_TICKS)) {
      DEBUG_LOG("Entering ring fill...");
      Menus_openMenuSelection();
    }
    return;
  }

  if (now < (CONSECUTIVE_WINDOW_TICKS * DEVICE_LOCK_CLICKS) && Button_onConsecutivePresses(g_pButton, DEVICE_LOCK_CLICKS - 1)) {
#ifdef VORTEX_LIB
    if (!VortexEngine_isSleeping()) {
      return;
    }
#endif
    Modes_setLocked(true, true);
    VortexEngine_enterSleep(true);
    return;
  }

  if ((Button_onRelease(g_pButton) && g_autoCycle) || Button_onConsecutivePresses(g_pButton, AUTO_CYCLE_MODES_CLICKS)) {
    g_autoCycle = !g_autoCycle;
    Leds_holdAll(g_autoCycle ? RGB_COLOR(RGB_GREEN) : RGB_COLOR(RGB_RED));
  }

  if (g_autoCycle && (Modes_lastSwitchTime() + AUTO_RANDOM_DELAY < now)) {
    Modes_nextMode();
  }

  Modes_play();
}

bool VortexEngine_serializeVersion(ByteStream *stream)
{
  if (!ByteStream_serialize8(stream, (uint8_t)VORTEX_VERSION_MAJOR)) {
    return false;
  }
  if (!ByteStream_serialize8(stream, (uint8_t)VORTEX_VERSION_MINOR)) {
    return false;
  }
  return true;
}

bool VortexEngine_checkVersion(uint8_t major, uint8_t minor)
{
  if (major != VORTEX_VERSION_MAJOR) {
    return false;
  }
  return true;
}

Mode *VortexEngine_curMode(void)
{
#ifdef VORTEX_LIB
  return Modes_curMode();
#else
  return NULL;
#endif
}

void VortexEngine_toggleForceSleep(bool enabled)
{
  g_forceSleepEnabled = enabled;
}

void VortexEngine_setAutoCycle(bool enabled)
{
  g_autoCycle = enabled;
}

#ifdef VORTEX_LIB
uint32_t VortexEngine_totalStorageSpace(void)
{
  return STORAGE_SIZE;
}

uint32_t VortexEngine_savefileSize(void)
{
  return Storage_lastSaveSize();
}

bool VortexEngine_isSleeping(void)
{
  return g_sleeping;
}
#endif

void VortexEngine_enterSleep(bool save)
{
  DEBUG_LOG("Sleeping");
  if (save) {
    Modes_setStartupMode(Modes_curModeIndex());
    Modes_saveStorage();
  }
  Leds_clearAll();
  Leds_update();
#ifdef VORTEX_EMBEDDED
  VortexEngine_clearOutputPins();
  VortexEngine_enableMOSFET(false);
  Time_delayMicroseconds(250);
  TCB0.INTCTRL = 0;
  TCB0.CTRLA = 0;
  Button_enableWake(g_pButton);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  g_sleeping = true;
  sleep_mode();
#else
  g_sleeping = true;
#endif
}

void VortexEngine_wakeup(bool reset)
{
  DEBUG_LOG("Waking up");
#ifdef VORTEX_EMBEDDED
  VortexEngine_enableMOSFET(true);
  if (reset) {
    _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
  }
#else
  g_sleeping = false;
  if (reset) {
    VortexEngine_cleanup();
    VortexEngine_init();
  }
#endif
}

#ifdef VORTEX_EMBEDDED
ISR(TCB0_INT_vect)
{
  TCB0.INTFLAGS = TCB_CAPT_bm;
  VortexEngine_tick();
}

void VortexEngine_clearOutputPins(void)
{
  PORTA.DIRCLR = 0xFF;
  PORTA.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.DIRCLR = 0xFF;
  PORTB.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.DIRCLR = 0xFF;
  PORTC.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
}

void VortexEngine_enableMOSFET(bool enabled)
{
  PORTC.DIRSET |= PIN4_bm;
  if (enabled) {
    PORTC.OUTSET |= PIN4_bm;
  } else {
    PORTC.OUTCLR |= PIN4_bm;
  }
}
#endif

#if COMPRESSION_TEST == 1
#include <string.h>
#include <stdio.h>
#include "Colors/Colorset.h"
#include "Memory/Memory.h"
#include "Random/Random.h"
void VortexEngine_compressionTest(void)
{
  Random rand;
  Random_initWithSeed(&rand, 0xdeadbeef);
  ByteStream stream;
  ByteStream_init(&stream, 0, NULL);
  Modes_clearModes();
  for (uint32_t len = 1; len < 4096; ++len) {
    uint8_t *buf = (uint8_t *)vcalloc(1, len + 1);
    if (!buf) continue;
    ByteStream modeStream;
    ByteStream_init(&modeStream, 0, NULL);
    Modes_serialize(&modeStream);
    while (ByteStream_size(&modeStream) < len) {
      Mode tmpMode;
      Mode_init(&tmpMode);
      Mode_setPatternID(&tmpMode, (PatternID)(len % PATTERN_COUNT));
      Colorset set;
      Colorset_init(&set);
      Colorset_randomizeColors(&set, &rand, 8, COLOR_MODE_EVENLY_SPACED);
      Mode_setColorset(&tmpMode, &set);
      Modes_addMode(&tmpMode);
      ByteStream_clear(&modeStream);
      Modes_serialize(&modeStream);
    }
    // ... copy stream etc
    ByteStream_destroy(&modeStream);
    free(buf);
  }
  Modes_clearModes();
  DEBUG_LOG("Success testing compression");
  ByteStream_destroy(&stream);
}
#endif

#if SERIALIZATION_TEST == 1
// ... serialization test
#endif

#if TIMER_TEST == 1
// ... timer test
#endif
