#include "VortexEngine.h"

#include "Infrared/IRReceiver.h"
#include "Infrared/IRSender.h"
#include "Storage/Storage.h"
#include "Buttons/Buttons.h"
#include "Time/TimeControl.h"
#include "Time/Timings.h"
#include "Serial/ByteStream.h"
#include "Modes/Modes.h"
#include "Menus/Menus.h"
#include "Modes/Mode.h"
#include "Leds/Leds.h"

#ifdef VORTEX_ARDUINO
#include <avr/interrupt.h>
#include <avr/sleep.h>
#endif

#ifdef VORTEX_LIB
// bool in vortexlib to simulate sleeping
bool VortexEngine::m_sleeping = false;
#endif

bool VortexEngine::init()
{
#ifdef VORTEX_ARDUINO
  // clear the output pins to initialize everything
  clearOutputPins();
  // open the mosfet so that power can flow to the leds
  enableMOSFET(true);
#endif

  // all of the global controllers
  if (!Time::init()) {
    DEBUG_LOG("Time failed to initialize");
    return false;
  }
  if (!Storage::init()) {
    DEBUG_LOG("Storage failed to initialize");
    return false;
  }
  if (!IRReceiver::init()) {
    DEBUG_LOG("IRReceiver failed to initialize");
    return false;
  }
  if (!IRSender::init()) {
    DEBUG_LOG("IRSender failed to initialize");
    return false;
  }
  if (!Leds::init()) {
    DEBUG_LOG("Leds failed to initialize");
    return false;
  }
  if (!Buttons::init()) {
    DEBUG_LOG("Buttons failed to initialize");
    return false;
  }
  if (!Menus::init()) {
    DEBUG_LOG("Menus failed to initialize");
    return false;
  }
  if (!Modes::init()) {
    DEBUG_LOG("Settings failed to initialize");
    return false;
  }

#if COMPRESSION_TEST == 1
  compressionTest();
#endif

#if SERIALIZATION_TEST == 1
  serializationTest();
#endif

#if TIMER_TEST == 1
  timerTest();
#endif

  return true;
}

void VortexEngine::cleanup()
{
#ifdef VORTEX_LIB
  // cleanup in reverse order
  // NOTE: the arduino doesn't actually cleanup,
  //       but the test frameworks do
  Modes::cleanup();
  Menus::cleanup();
  Buttons::cleanup();
  Leds::cleanup();
  IRSender::cleanup();
  IRReceiver::cleanup();
  Storage::cleanup();
  Time::cleanup();
#endif
}

void VortexEngine::tick()
{
#ifdef VORTEX_LIB
  if (m_sleeping) {
    // directly poll the button to wakeup so that we do't need Buttons::update()
    if (g_pButton->check()) {
      wakeup();
    }
    return;
  }
#endif
  // handle any fatal errors that may have occurred
  // but only if the error blinker is enabled
#if VORTEX_ERROR_BLINK == 1
  if (getError() != ERROR_NONE) {
    // just blink the error and don't run anything
    blinkError();
    return;
  }
#endif

  // tick the current time counter forward
  Time::tickClock();

  // poll the button(s) and update the button object states
  Buttons::update();

  // run the main logic for the engine
  runMainLogic();

  // update the leds
  Leds::update();
}

// warning this function is quite heavy at this point, it could probably be split
void VortexEngine::runMainLogic()
{
  if (Modes::locked()) {
    // 5 fast clicks will unlock the device
    if (g_pButton->consecutivePresses() >= DEVICE_LOCK_CLICKS) {
      // unlock and just wakeup to reset
      Modes::setLocked(false);
      wakeup();
    }
    // if the device is locked it only stays awake for the wake window
    // the user must press the button X times in this window to unlock
    if (Time::getCurtime() > UNLOCK_WAKE_WINDOW_TICKS) {
      // go back to sleep
      enterSleep();
    } else {
      // if you want to indicate the window where sleep can be woken
      //Leds::setIndex(LED_1, RGB_DIM_RED);
    }
    // don't do anything else while locked
    return;
  }

  // the device is not locked, proceed with regular logic

  // if the button hasn't been released since turning on then there is custom logic
  if (g_pButton->releaseCount() == 0) {
    // if the button is held for 2 seconds from off, switch the brigness scale
    if (Time::getCurtime() == SHORT_CLICK_THRESHOLD_TICKS && g_pButton->isPressed()) {
      // update brightness and save the changes
      Leds::setBrightness(Leds::getBrightness() == DEFAULT_BRIGHTNESS ? DEFAULT_DIMNESS : DEFAULT_BRIGHTNESS);
      Modes::saveStorage();
    }
    // do nothing till the user releases the button... No menus mothing
    Modes::play();
    return;
  }

  // finally the user has released the button after initially turning it on,
  // just run the regular main logic of the system

  // first look for the force-sleep and instant on/off toggle
  uint32_t holdTime = g_pButton->holdDuration();
  // force-sleep check takes precedence above all
  if (holdTime >= FORCE_SLEEP_THRESHOLD_TICKS) {
    // as long as they hold down past this threshold just turn off
    if (g_pButton->isPressed()) {
      Leds::clearAll();
      return;
    }
    // but as soon as they actually release put the device to sleep and also
    // toggle the instant on/off if they were at the main menu
    if (g_pButton->onRelease()) {
      if (!Menus::checkInMenu()) {
        Modes::setInstantOnOff(!Modes::instantOnOffEnabled());
      }
      enterSleep();
    }
    return;
  }

  // run the menus to see if they are open and need to do anything
  if (Menus::run()) {
    // if they return true that means the menus are open and rendering so just return
    return;
  }

  // if the user releases the button after the sleep threshold and
  // we're still in menu state not open, then we can go to sleep
  if (g_pButton->onRelease() && holdTime >= SLEEP_ENTER_THRESHOLD_TICKS) {
    // enter sleep mode
    enterSleep();
    return;
  }

  // if the button is just held beyond the sleep threshold then
  if (g_pButton->isPressed() && holdTime >= SLEEP_ENTER_THRESHOLD_TICKS) {
    // clear all the leds for a short moment
    Leds::clearAll();
    // then oncethe user holds past the sleep window threshold open up the menus
    if (holdTime >= (SLEEP_ENTER_THRESHOLD_TICKS + SLEEP_WINDOW_THRESHOLD_TICKS)) {
      // open the menu selection area
      DEBUG_LOG("Entering ring fill...");
      Menus::openMenuSelection();
    }
    // don't play the modes because the user is going into menus
    return;
  }

  // lastly check if we are locking the device, which can only happen at the
  // main modes playing
  if (g_pButton->consecutivePresses() >= DEVICE_LOCK_CLICKS) {
    // lock and just go to sleep
    Modes::setLocked(true);
    enterSleep();
    return;
  }

  // otherwise finally just play the modes like normal
  Modes::play();
}

void VortexEngine::serializeVersion(ByteStream &stream)
{
  // serialize the vortex version
  stream.serialize((uint8_t)VORTEX_VERSION_MAJOR);
  stream.serialize((uint8_t)VORTEX_VERSION_MINOR);
}

bool VortexEngine::checkVersion(uint8_t major, uint8_t minor)
{
  if (major != VORTEX_VERSION_MAJOR) {
    return false;
  }
  // minor version doesn't matter
  return true;
}

Mode *VortexEngine::curMode()
{
#ifdef VORTEX_LIB
  return Modes::curMode();
#else
  // don't need this outside vortex lib
  return nullptr;
#endif
}

#ifdef VORTEX_LIB
uint32_t VortexEngine::totalStorageSpace()
{
  return STORAGE_SIZE;
}

uint32_t VortexEngine::savefileSize()
{
  return Storage::lastSaveSize();
}
#endif

void VortexEngine::enterSleep()
{
  DEBUG_LOG("Sleeping");
  // clear all the leds
  Leds::clearAll();
  Leds::update();
#ifdef VORTEX_ARDUINO
  // init the output pins to prevent any floating pins
  clearOutputPins();
  // close the mosfet so that power cannot flow to the leds
  enableMOSFET(false);
  // delay for a bit to let the mosfet close and leds turn off
  delayMicroseconds(250);
  // this is an ISR that runs in the timecontrol system to handle
  // millis and micros, it will wake the device up after some time
  // if it isn't disabled
  TCD0.INTCTRL = 0;
  TCD0.CTRLA = 0;
  // Set wake interrupt on falling edges
  PORTB.PIN2CTRL = 0x3;
  // Set sleep mode to POWER DOWN mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // Enable sleep mode, but not going to sleep yet
  sleep_enable();
  // enter sleep
  sleep_cpu();
#else
  m_sleeping = true;
#endif
}

void VortexEngine::wakeup()
{
  DEBUG_LOG("Waking up");
#ifdef VORTEX_ARDUINO
  // turn the LED mosfet back on
  enableMOSFET(true);
  // just reset
  _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
#else
  // need to fake the reset in vortexlib, lol this works I guess
  cleanup();
  init();
  m_sleeping = false;
#endif
}

#ifdef VORTEX_ARDUINO
void VortexEngine::clearOutputPins()
{
  // Set all pins to input with pull-ups
  PORTA.DIRCLR = 0xFF;
  PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
  PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
  PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
  PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
  PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
  PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
  PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
  PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
  PORTB.DIRCLR = 0xFF;
  PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
  PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
  PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
  PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
  PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
  PORTB.PIN5CTRL = PORT_PULLUPEN_bm;
  PORTB.PIN6CTRL = PORT_PULLUPEN_bm;
  PORTB.PIN7CTRL = PORT_PULLUPEN_bm;
  PORTC.DIRCLR = 0xFF;
  PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN2CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN4CTRL = PORT_PULLUPEN_bm;
  PORTC.PIN5CTRL = PORT_PULLUPEN_bm;
}

void VortexEngine::enableMOSFET(bool enabled)
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

// interrupt handler to wakeup device on button press
ISR(PORTB_PORT_vect)
{
  if (!(PORTB.INTFLAGS & PIN2_bm)) {
    // don't trigger unless it was from the button press
    return;
  }
  // handled
  PORTB.INTFLAGS = PIN2_bm;
  // turn off interrupt
  PORTB.PIN2CTRL &= ~PORT_ISC_gm;
  // wakeup
  VortexEngine::wakeup();
}
#endif

#if COMPRESSION_TEST == 1
#include <string.h>
#include <stdio.h>
#include "Colors/Colorset.h"
#include "Memory/Memory.h"
#include "Random/Random.h"
void VortexEngine::compressionTest()
{
  // always use same seed
  Random rand(0xdeadbeef);
  ByteStream stream;
  Modes::clearModes();
  for (uint32_t len = 1; len < 4096; ++len) {
    uint8_t *buf = (uint8_t *)vcalloc(1, len + 1);
    if (!buf) {
      continue;
    }
    ByteStream modeStream;
    Modes::serialize(modeStream);
    while (modeStream.size() < len) {
      Mode tmpMode;
      tmpMode.setPattern((PatternID)(len % PATTERN_COUNT));
      Colorset set;
      set.randomizeColorTheory(rand, 8);
      tmpMode.setColorset(set);
      Modes::addMode(&tmpMode);
      modeStream.clear();
      Modes::serialize(modeStream);
    }
    stream = modeStream;
#if 0
    for (uint32_t i = 0; i < len; ++i) {
      buf[i] = (uint8_t)i&0xFF;
    }
    stream.init(len, buf);
#endif
    stream.compress();
    int complen = stream.size();
    stream.decompress();
    if (memcmp(stream.data(), modeStream.data(), modeStream.size()) != 0) {
      ERROR_LOGF("Buffers not equal: %u", modeStream.size());
      INFO_LOG("\t");
      for (uint32_t i = 0; i < len; ++i) {
        printf("%02x ", modeStream.data()[i]);
        if (i > 0 && ((i + 1) % 32) == 0) {
          printf("\r\n\t");
        }
      }
      return;
    }
    if (modeStream.size() != (size_t)complen) {
      DEBUG_LOGF("Success %u compressed to %u", modeStream.size(), complen);
    }
    free(buf);
  }
  Modes::clearModes();
  DEBUG_LOG("Success testing compression");
}
#endif

#if SERIALIZATION_TEST == 1
#include "Modes/Mode.h"
#include "Colors/Colorset.h"
#include "Patterns/Pattern.h"
#include "Patterns/PatternArgs.h"
#include "Patterns/PatternBuilder.h"
#include "Random/Random.h"
#include <stdio.h>
void VortexEngine::serializationTest()
{
  Colorset bigSet;
  Random rand;
  for (uint32_t i = 0; i < MAX_COLOR_SLOTS; ++i) {
    bigSet.addColorWithValueStyle(rand, i * 31, 255, Colorset::VAL_STYLE_ALTERNATING, MAX_COLOR_SLOTS);
  }
  DEBUG_LOG("== Beginning Serialization Test ==");
  for (PatternID patternID = PATTERN_FIRST; patternID < PATTERN_COUNT; ++patternID) {
    PatternArgs defaults = PatternBuilder::getDefaultArgs(patternID);
    PatternArgs args(0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x11, 0x22, 0x33);
    args.numArgs = defaults.numArgs;
    Mode tmpMode(patternID, &args, &bigSet);
    tmpMode.init();
    ByteStream buffer;
    tmpMode.serialize(buffer);
    if (!buffer.size()) {
      ERROR_LOGF("ERROR!! Buffer empty after serialize on %u", patternID);
      return;
    }
    Mode tmpMode2;
    tmpMode2.unserialize(buffer);
    if (!buffer.unserializerAtEnd()) {
      ERROR_LOGF("ERROR!! Unserializer still has data on %u:", patternID);
      uint8_t byte = 0;
      uint32_t count = 0;
      while (buffer.unserialize(&byte)) {
        INFO_LOGF("%02x ", byte);
        if ((++count % 32) == 0) {
          INFO_LOG("\n");
        }
      }
      INFO_LOG("\n");
      return;
    }
    if (!tmpMode.equals(&tmpMode2)) {
      ERROR_LOGF("ERROR!! Modes are not equal on %u", patternID);
      return;
    }
    PatternArgs pulledArgs;
    tmpMode2.getPattern()->getArgs(pulledArgs);
    if (pulledArgs != args) {
      DEBUG_LOGF("ERROR!! Pattern args are not equal on %u", patternID);
      return;
    }
    DEBUG_LOGF("Success pattern %u serialized cleanly", patternID);
  }
  DEBUG_LOG("All patterns serialized successfully...");
  DEBUG_LOG("Attempting full serialization of all modes");
  Modes::clearModes();
  for (PatternID patternID = PATTERN_FIRST; patternID < PATTERN_COUNT; ++patternID) {
    if (!Modes::addMode(patternID, nullptr, &bigSet)) {
      ERROR_LOGF("ERROR!! Failed to add mode %u", patternID);
      return;
    }
  }
  ByteStream buffer;
  Modes::serialize(buffer);
  if (!buffer.size()) {
    ERROR_LOG("ERROR!! Buffer empty after modes serialize");
    return;
  }
  Modes::clearModes();
  Modes::unserialize(buffer);
  if (!buffer.unserializerAtEnd()) {
    ERROR_LOG("ERROR!! Unserializer still has data after modes unserialize:");
    uint8_t byte = 0;
    uint32_t count = 0;
    while (buffer.unserialize(&byte)) {
      INFO_LOGF("%02x ", byte);
      if ((++count % 32) == 0) {
        printf("\n");
      }
    }
    printf("\n");
    return;
  }
  Modes::clearModes();
  DEBUG_LOG("Success all modes serialized cleanly");
  DEBUG_LOG("== SUCCESS RUNNING SERIALIZATION TEST ==");
}
#endif

#if TIMER_TEST == 1
#include "Time/TimeControl.h"
#include "Time/timer.h"
void VortexEngine::timerTest()
{
  DEBUG_LOG("== BEGINNING TIMER TESTS ==");
  Time::test();
  Timer::test();
  DEBUG_LOG("== SUCCESS TIMER TESTS PASSED ==");
}
#endif
