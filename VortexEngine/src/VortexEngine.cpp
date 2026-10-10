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
#include "Serial/ByteStream.h"
#include "Modes/Modes.h"
#include "Patterns/PatternBuilder.h"
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

// the sleep state, lives here in the engine
static volatile bool VortexEngine_m_sleeping = false;
// whether the 'force sleep' option is available (hold down for long time)
static bool VortexEngine_m_forceSleepEnabled = true;

// auto cycling
static bool VortexEngine_m_autoCycle = false;

bool VortexEngine_isSleeping()
{
  return VortexEngine_m_sleeping;
}

void VortexEngine_toggleForceSleep(bool enabled)
{
  VortexEngine_m_forceSleepEnabled = enabled;
}

void VortexEngine_setAutoCycle(bool enabled)
{
  VortexEngine_m_autoCycle = enabled;
}

bool VortexEngine_init()
{
#ifdef VORTEX_EMBEDDED
  // clear the output pins to initialize everything
  VortexEngine_clearOutputPins();
#endif

  // all of the global controllers
  if (!ByteStream_initArena()) {
    DEBUG_LOG("ByteStream arena failed to initialize");
    return false;
  }
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
  if (!Buttons_init()) {
    DEBUG_LOG("Buttons failed to initialize");
    return false;
  }
  if (!PatternBuilder_init()) {
    DEBUG_LOG("PatternBuilder failed to initialize");
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
  // setup TCB0 to track micros() and run ticks
  TCB0.CCMP = 10000;
  TCB0.INTCTRL = TCB_CAPT_bm;
  // Set clock source to CPU divided by 1, Keep running in sleep mode, Enable TCB
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_RUNSTDBY_bm | TCB_ENABLE_bm;
  // set the state of the mosfet based on whether the chip is locked or not
  VortexEngine_enableMOSFET(!Modes_locked());
  // setup sleep mode for standby
  set_sleep_mode(SLEEP_MODE_STANDBY);
  // enable interrupts
  sei();
  // standby indefinitely while the ISR runs VortexEngine::tick
  while (!VortexEngine_m_sleeping) {
    sleep_mode();
  }
#endif

  return true;
}

void VortexEngine_cleanup()
{
  // cleanup in reverse order
  // NOTE: the embedded doesn't actually cleanup,
  //       but the test frameworks do
#ifdef VORTEX_LIB
  // vortexlib fakes the reset on wake with cleanup then init
  VortexEngine_m_forceSleepEnabled = true;
  VortexEngine_m_autoCycle = false;
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

void VortexEngine_tick()
{
#ifdef VORTEX_LIB
  if (VortexEngine_m_sleeping) {
    // update the buttons to check for wake
    Buttons_update();
    // several fast clicks will unlock the device
    if (Modes_locked() && Button_onConsecutivePresses(g_pButton, DEVICE_LOCK_CLICKS - 1)) {
      // turn off the lock flag and save it to disk
      Modes_setLocked(false, true);
    }
    // check for any kind of press to wakeup
    if (Button_check(g_pButton) || Button_onRelease(g_pButton) || !Vortex::sleepEnabled()) {
      VortexEngine_wakeup(true);
    }
    return;
  }
#endif

  // tick the current time counter forward
  Time_tickClock();

  // poll the button(s) and update the button object states
  Buttons_update();

  // run the main logic for the engine
  VortexEngine_runMainLogic();

  // update the leds
  Leds_update();
}

// warning this function is quite heavy at this point, it could probably be split
void VortexEngine_runMainLogic()
{
  // the current tick
  uint32_t now = Time_getCurtime();

  // if the device is locked then that takes priority over all, while locked the
  // device will only listen for clicks to wakeup momentarily then go back to sleep
  if (Modes_locked()) {
    // several fast clicks will unlock the device
    if (Button_onConsecutivePresses(g_pButton, DEVICE_LOCK_CLICKS - 1)) {
      // turn off the lock flag and save it to disk
      Modes_setLocked(false, true);
#ifdef VORTEX_EMBEDDED
      // then enable the mosfet
      VortexEngine_enableMOSFET(true);
#endif
    } else if (now > (CONSECUTIVE_WINDOW_TICKS * DEVICE_LOCK_CLICKS)) {
      // go back to sleep if they don't unlock in time, don't save
      VortexEngine_enterSleep(false);
    }
    // OPTIONAL: render a dim led during unlock window waiting for clicks?
    //Leds_setIndex(LED_1, RGB_RED4);
    Leds_clearAll();
    // don't do anything else while locked, just return
    return;
  }

  // the device is not locked, proceed with regular logic

  // if the button hasn't been released since turning on then there is custom logic
  if (Button_releaseCount(g_pButton) == 0) {
    if (!Modes_load()) {
      return;
    }
    // if the button is held for 2 seconds from off, switch to on click mode on
    // the last mode shown before sleep
    if (!Modes_keychainModeEnabled() && now == ONE_CLICK_THRESHOLD_TICKS && Button_isPressed(g_pButton)) {
      // whether oneclick mode is now enabled
      bool isEnabledNow = !Modes_oneClickModeEnabled();
      // toggle one click mode
      Modes_setOneClickMode(isEnabledNow, true);
      // if we turned it on then switch to that mode
      if (isEnabledNow) {
        Modes_switchToStartupMode();
      } else {
        Modes_setCurMode(0);
      }
      // flash either low white or dim white2 to indicate
      // whether one-click mode has been turned on or off
      Leds_holdAll(isEnabledNow ? RGB_WHITE0 : RGB_WHITE5);
    }
    return;
  }

#ifdef VORTEX_EMBEDDED
  // originally this check was believed to protect against ESD but not so sure
  // anymore, it's clear that it stops the chip from turning on when it initially
  // receives power but ESD tests fail now (put chip in sandwich bag and shake it).
  // This is now where initialization runs right after a fresh firmware flash.
  // The first tick of the engine is 1 not 0 because oops.
  if (now == 1) {
    // This check is for whether a new firmware was just flashed, if a new
    // firmware was flashed then write out a new save header
    if (Modes_getFlag(MODES_FLAG_NEW_FIRMWARE)) {
      // reset the flags back to normal
      Modes_resetFlags();
      // try to load the modes and hope it works, need the num modes to write a
      // new save header and they may have backed up and restored a custom number
      // of modes
      Modes_load();
      // then save the new header with current version and num modes etc
      Modes_saveHeader();
    }
    // then just gracefully go back to sleep to prevent the chip from turning
    // on randomly when it receives power
    VortexEngine_enterSleep(false);
    return;
  }
#endif

  if (!Modes_load()) {
    return;
  }

  // finally the user has released the button after initially turning it on,
  // just run the regular main logic of the system

  // re-enter keychain mode if it was never disabled
  if (Modes_keychainModeEnabled() && !Menus_checkInMenu()) {
    // switch to the last mode we were on
    Modes_switchToStartupMode();
    // enter keychain mode menu
    Menus_openMenu(MENU_GLOBAL_BRIGHTNESS, true);
  }

  // first look for the force-sleep and instant on/off toggle
  const uint32_t holdTime = Button_holdDuration(g_pButton);
  // force-sleep check takes precedence above all, but it does not run when keychain mode is enabled
  if (VortexEngine_m_forceSleepEnabled && holdTime >= FORCE_SLEEP_THRESHOLD_TICKS) {
    // as long as they hold down past this threshold just turn off
    if (Button_isPressed(g_pButton)) {
      Leds_clearAll();
      return;
    }
    // but as soon as they actually release put the device to sleep
    if (Button_onRelease(g_pButton)) {
      // do not save on force sleep
      VortexEngine_enterSleep(false);
    }
    return;
  }

  // run the menus to see if they are open and need to do anything
  if (Menus_run()) {
    // if they return true that means the menus are open and rendering so just return
    return;
  }

  // if the user releases the button after the sleep threshold and
  // we're still in menu state not open, then we can go to sleep
  if (Button_onRelease(g_pButton) && holdTime >= SLEEP_ENTER_THRESHOLD_TICKS) {
    // enter sleep mode
    VortexEngine_enterSleep(true);
    return;
  }

  // if the button is just held beyond the sleep threshold then
  if (Button_isPressed(g_pButton) && holdTime >= SLEEP_ENTER_THRESHOLD_TICKS) {
    // clear all the leds for a short moment
    Leds_clearAll();
    // then oncethe user holds past the sleep window threshold open up the menus
    if (holdTime >= (SLEEP_ENTER_THRESHOLD_TICKS + SLEEP_WINDOW_THRESHOLD_TICKS)) {
      // open the menu selection area
      DEBUG_LOG("Entering ring fill...");
      Menus_openMenuSelection();
    }
    // don't play the modes because the user is going into menus
    return;
  }

  // lastly check if we are locking the device, which can only happen if they click the
  // button 5 times quickly when the device was off, so 4 times in the first x ticks because
  // the first click was used to wake the device and isn't counted in the consecutive clicks
  if (now < (CONSECUTIVE_WINDOW_TICKS * DEVICE_LOCK_CLICKS) && Button_onConsecutivePresses(g_pButton, DEVICE_LOCK_CLICKS - 1)) {
#ifdef VORTEX_LIB
    if (!Vortex::lockEnabled()) {
      return;
    }
#endif
    // lock and just go to sleep, don't need to reset consecutive press counter here
    Modes_setLocked(true, true);
    VortexEngine_enterSleep(true);
    return;
  }

  // toggle auto cycle mode with many clicks at main modes
  if ((Button_onRelease(g_pButton) && VortexEngine_m_autoCycle) || Button_onConsecutivePresses(g_pButton, AUTO_CYCLE_MODES_CLICKS)) {
    VortexEngine_m_autoCycle = !VortexEngine_m_autoCycle;
    Leds_holdAll(VortexEngine_m_autoCycle ? RGB_GREEN : RGB_RED);
  }

  // if auto cycle is enabled and the last switch was more than the delay ago
  if (VortexEngine_m_autoCycle && (Modes_lastSwitchTime() + AUTO_RANDOM_DELAY < now)) {
    // then switch to the next mode automatically
    Modes_nextMode();
  }

  // otherwise just play the modes
  Modes_play();
}

bool VortexEngine_serializeVersion(ByteStream *stream)
{
  // serialize the vortex version
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
  // minor version doesn't matter
  return true;
}

Mode *VortexEngine_curMode()
{
#ifdef VORTEX_LIB
  return Modes_curMode();
#else
  // don't need this outside vortex lib
  return nullptr;
#endif
}

#ifdef VORTEX_LIB
uint32_t VortexEngine_totalStorageSpace()
{
  return STORAGE_SIZE;
}

uint32_t VortexEngine_savefileSize()
{
  return Storage_lastSaveSize();
}
#endif

void VortexEngine_enterSleep(bool save)
{
  DEBUG_LOG("Sleeping");
  if (save) {
    // update the startup mode when going to sleep
    Modes_setStartupMode(Modes_curModeIndex());
    // save anything that hasn't been saved
    Modes_saveStorage();
  }
  // clear all the leds
  Leds_clearAll();
  Leds_update();
#ifdef VORTEX_EMBEDDED
  // init the output pins to prevent any floating pins
  VortexEngine_clearOutputPins();
  // close the mosfet so that power cannot flow to the leds
  VortexEngine_enableMOSFET(false);
  // delay for a bit to let the mosfet close and leds turn off
  Time_delayMicroseconds(250);
  // this is an ISR that runs in the timecontrol system to handle
  // micros, it will wake the device up periodically
  TCB0.INTCTRL = 0;
  TCB0.CTRLA = 0;
  // Enable wake on interrupt for the button
  Button_enableWake(g_pButton);
  // Set sleep mode to POWER DOWN mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // enable the sleep bool right before we enter sleep, this will allow
  // the main loop to break and return
  VortexEngine_m_sleeping = true;
  // enter sleep
  sleep_mode();
#else
  // enable the sleep bool
  VortexEngine_m_sleeping = true;
#endif
}

#ifdef VORTEX_EMBEDDED
void VortexEngine_outOfMemory(void)
{
  // show red for a second so the failure is visible
  Leds_setAll(RGB_RED);
  Leds_update();
  for (uint16_t i = 0; i < 1000; ++i) {
    Time_delayMicroseconds(1000);
  }
  // then turn off without saving, saving would need to allocate
  VortexEngine_enterSleep(false);
  // the button wakes the chip but this usually runs inside the tick
  // interrupt so the wake interrupt can't run yet, just reset here instead
  // of returning to whatever just failed to allocate
  VortexEngine_wakeup(true);
}
#endif

void VortexEngine_wakeup(bool reset)
{
  DEBUG_LOG("Waking up");
#ifdef VORTEX_EMBEDDED
  // turn the LED mosfet back on
  VortexEngine_enableMOSFET(true);
  if (reset) {
    // just reset
    _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
  }
#else
  // need to fake the reset in vortexlib, lol this works I guess
  VortexEngine_m_sleeping = false;
  if (reset) {
    VortexEngine_cleanup();
    VortexEngine_init();
  }
#endif
}

#ifdef VORTEX_EMBEDDED
// main tick function
ISR(TCB0_INT_vect)
{
  // Clear interrupt flag
  TCB0.INTFLAGS = TCB_CAPT_bm;
  VortexEngine_tick();
}

void VortexEngine_clearOutputPins()
{
  // Set all pins to input with pull-ups
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
}

void VortexEngine_enableMOSFET(bool enabled)
{
  PORTC.DIRSET |= PIN4_bm;
  if (enabled) {
    // Set Mosfet pin (PC4) to output and set it HIGH
    PORTC.OUTSET |= PIN4_bm;
  } else {
    // Set Mosfet pin (PC4) to output and set it LOW
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
void VortexEngine_compressionTest()
{
  // always use same seed
  Random rand;
  Random_makeSeed(&rand, 0xdeadbeef);
  ByteStream stream;
  ByteStream_make(&stream);
  Modes_clearModes();
  for (uint32_t len = 1; len < 4096; ++len) {
    ByteStream modeStream;
    ByteStream_make(&modeStream);
    Modes_serialize(&modeStream);
    while (ByteStream_size(&modeStream) < len) {
      Mode tmpMode;
      Mode_init(&tmpMode);
      Mode_setPattern(&tmpMode, (PatternID)(len % PATTERN_COUNT), LED_ANY, NULL, NULL);
      Colorset set;
      Colorset_make(&set);
      Colorset_randomizeColors(&set, &rand, 8, THEORY);
      Mode_setColorset(&tmpMode, &set, LED_ALL);
      bool added = Modes_addMode(&tmpMode);
      Mode_destroy(&tmpMode);
      if (!added) {
        // the mode pools are fixed size so the test ends once they are full
        DEBUG_LOGF("Mode pool full at length %u, stopping", len);
        ByteStream_destroy(&modeStream);
        ByteStream_destroy(&stream);
        Modes_clearModes();
        DEBUG_LOG("Success testing compression");
        return;
      }
      ByteStream_clear(&modeStream);
      Modes_serialize(&modeStream);
    }
    ByteStream_set(&stream, &modeStream);
    ByteStream_compress(&stream);
    uint32_t complen = ByteStream_size(&stream);
    ByteStream_decompress(&stream);
    if (ByteStream_size(&stream) != ByteStream_size(&modeStream) ||
        memcmp(ByteStream_data(&stream), ByteStream_data(&modeStream), ByteStream_size(&modeStream)) != 0) {
      ERROR_LOGF("Buffers not equal: %u", ByteStream_size(&modeStream));
      INFO_LOG("\t");
      const uint8_t *data = (const uint8_t *)ByteStream_data(&modeStream);
      for (uint32_t i = 0; i < ByteStream_size(&modeStream); ++i) {
        printf("%02x ", data[i]);
        if (i > 0 && ((i + 1) % 32) == 0) {
          printf("\r\n\t");
        }
      }
      ByteStream_destroy(&modeStream);
      ByteStream_destroy(&stream);
      return;
    }
    if (ByteStream_size(&modeStream) != complen) {
      DEBUG_LOGF("Success %u compressed to %u", ByteStream_size(&modeStream), complen);
    }
    ByteStream_destroy(&modeStream);
  }
  ByteStream_destroy(&stream);
  Modes_clearModes();
  DEBUG_LOG("Success testing compression");
}
#endif

#if SERIALIZATION_TEST == 1
#include <string.h>
#include "Modes/Mode.h"
#include "Colors/Colorset.h"
#include "Patterns/Pattern.h"
#include "Patterns/PatternArgs.h"
#include "Patterns/PatternBuilder.h"
#include "Random/Random.h"
#include <stdio.h>
void VortexEngine_serializationTest()
{
  Colorset bigSet;
  Colorset_make(&bigSet);
  Random rand;
  Random_make(&rand);
  for (uint32_t i = 0; i < MAX_COLOR_SLOTS; ++i) {
    Colorset_addColorWithValueStyle(&bigSet, &rand, i * 31, 255, VAL_STYLE_ALTERNATING, MAX_COLOR_SLOTS, i);
  }
  DEBUG_LOG("== Beginning Serialization Test ==");
  for (PatternID patternID = PATTERN_FIRST; patternID < PATTERN_COUNT; ++patternID) {
    PatternArgs defaults;
    PatternBuilder_getDefaultArgs(patternID, &defaults);
    PatternArgs args;
    PatternArgs_make8(&args, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x11, 0x22, 0x33);
    args.numArgs = defaults.numArgs;
    Mode tmpMode;
    Mode_initPatternArgsSet(&tmpMode, patternID, &args, &bigSet);
    Mode_initPatterns(&tmpMode);
    ByteStream buffer;
    ByteStream_make(&buffer);
    Mode_serialize(&tmpMode, &buffer, LED_COUNT);
    Mode tmpMode2;
    Mode_init(&tmpMode2);
    bool ok = false;
    if (!ByteStream_size(&buffer)) {
      ERROR_LOGF("ERROR!! Buffer empty after serialize on %u", patternID);
    } else if (!Mode_unserialize(&tmpMode2, &buffer) || !ByteStream_unserializerAtEnd(&buffer)) {
      ERROR_LOGF("ERROR!! Unserializer still has data on %u:", patternID);
      uint8_t byte = 0;
      uint32_t count = 0;
      while (ByteStream_unserialize8(&buffer, &byte)) {
        INFO_LOGF("%02x ", byte);
        if ((++count % 32) == 0) {
          INFO_LOG("\n");
        }
      }
      INFO_LOG("\n");
    } else if (!Mode_equals(&tmpMode, &tmpMode2)) {
      ERROR_LOGF("ERROR!! Modes are not equal on %u", patternID);
    } else {
      // a pattern holds all of its args even when the defaults only set some
      // of them so compare against the args of the original pattern
      PatternArgs origArgs;
      PatternArgs_make0(&origArgs);
      Pattern_getArgs(Mode_getPattern(&tmpMode, LED_ANY), &origArgs);
      PatternArgs pulledArgs;
      PatternArgs_make0(&pulledArgs);
      Pattern_getArgs(Mode_getPattern(&tmpMode2, LED_ANY), &pulledArgs);
      if (PatternArgs_notEqual(&pulledArgs, &origArgs) ||
          memcmp(origArgs.args, args.args, args.numArgs) != 0) {
        DEBUG_LOGF("ERROR!! Pattern args are not equal on %u", patternID);
      } else {
        ok = true;
      }
    }
    ByteStream_destroy(&buffer);
    Mode_destroy(&tmpMode2);
    Mode_destroy(&tmpMode);
    if (!ok) {
      return;
    }
    DEBUG_LOGF("Success pattern %u serialized cleanly", patternID);
  }
  DEBUG_LOG("All patterns serialized successfully...");
  DEBUG_LOG("Attempting full serialization of all modes");
  Modes_clearModes();
  for (PatternID patternID = PATTERN_FIRST; patternID < PATTERN_COUNT; ++patternID) {
    if (!Modes_addMode(patternID, (const PatternArgs *)NULL, &bigSet)) {
      ERROR_LOGF("ERROR!! Failed to add mode %u", patternID);
      return;
    }
  }
  ByteStream buffer;
  ByteStream_make(&buffer);
  Modes_serialize(&buffer);
  if (!ByteStream_size(&buffer)) {
    ERROR_LOG("ERROR!! Buffer empty after modes serialize");
    ByteStream_destroy(&buffer);
    return;
  }
  Modes_clearModes();
  Modes_unserialize(&buffer);
  if (!ByteStream_unserializerAtEnd(&buffer)) {
    ERROR_LOG("ERROR!! Unserializer still has data after modes unserialize:");
    uint8_t byte = 0;
    uint32_t count = 0;
    while (ByteStream_unserialize8(&buffer, &byte)) {
      INFO_LOGF("%02x ", byte);
      if ((++count % 32) == 0) {
        printf("\n");
      }
    }
    printf("\n");
    ByteStream_destroy(&buffer);
    return;
  }
  ByteStream_destroy(&buffer);
  Modes_clearModes();
  DEBUG_LOG("Success all modes serialized cleanly");
  DEBUG_LOG("== SUCCESS RUNNING SERIALIZATION TEST ==");
}
#endif

#if TIMER_TEST == 1
#include "Time/TimeControl.h"
#include "Time/Timer.h"
void VortexEngine_timerTest()
{
  DEBUG_LOG("== BEGINNING TIMER TESTS ==");
  Time_test();
  Timer_test();
  DEBUG_LOG("== SUCCESS TIMER TESTS PASSED ==");
}
#endif
