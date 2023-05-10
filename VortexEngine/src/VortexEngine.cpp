#include "VortexEngine.h"

#include "Infrared/IRReceiver.h"
#include "Infrared/IRSender.h"
#include "Storage/Storage.h"
#include "Buttons/Buttons.h"
#include "Time/TimeControl.h"
#include "Time/Timings.h"
//#include "Serial/Serial.h"
#include "Serial/ByteStream.h"
#include "Modes/Modes.h"
#include "Menus/Menus.h"
#include "Modes/Mode.h"
#include "Leds/Leds.h"
//#include "Log/Log.h"

#ifdef VORTEX_ARDUINO
#include <avr/interrupt.h>
#include <avr/sleep.h>
#endif

bool VortexEngine::init()
{
  //// Set all pins to input with pull-ups
  //PORTA.DIRCLR = 0xFF;
  //PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
  //PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
  //PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
  //PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
  //PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
  //PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
  //PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
  //PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

  //PORTB.DIRCLR = 0xFF;
  //PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
  //PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
  //PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
  //PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
  //PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
  //PORTB.PIN5CTRL = PORT_PULLUPEN_bm;
  //PORTB.PIN6CTRL = PORT_PULLUPEN_bm;
  //PORTB.PIN7CTRL = PORT_PULLUPEN_bm;

  //// Set Mosfet pin (PB0) to output and set it HIGH
  //PORTB.DIRSET = PIN0_bm;
  //PORTB.OUTSET = PIN0_bm;

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
}

void VortexEngine::tick()
{
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

  // don't poll the button till some cycles have passed, this prevents
  // the wakeup from cycling to the next mode
  Buttons::check();

  // run the main logic for the engine
  runMainLogic();

  // update the leds
  Leds::update();
}

void VortexEngine::runMainLogic()
{
  // otherwise check for any press or hold to enter sleep or enter menus
  uint32_t holdTime = g_pButton->holdDuration();
  // force-sleep check
  if (holdTime >= FORCE_SLEEP_THRESHOLD_TICKS) {
    if (g_pButton->isPressed()) {
      Leds::clearAll();
    } else if (g_pButton->onLongClick()) {
      enterSleep();
    }
    return;
  }
  // if the menus are open and running then just return
  if (Menus::run()) {
    return;
  }
  // if the user releases the button after the sleep threshold and
  // we're still in menu state not open, then we can go to sleep
  if (g_pButton->onLongClick() && holdTime >= SLEEP_ENTER_THRESHOLD_TICKS) {
    // enter sleep mode, this won't return
    enterSleep();
  }
  if (g_pButton->isPressed() && holdTime >= SLEEP_ENTER_THRESHOLD_TICKS) {
    // and finally if the button is pressed then clear the leds if within
    // the sleep window and open the ring menu if past that
    Leds::clearAll();
    // then check to see if we've held long enough to enter the menu
    if (holdTime >= (SLEEP_ENTER_THRESHOLD_TICKS + SLEEP_WINDOW_THRESHOLD_TICKS)) {
      DEBUG_LOG("Entering ring fill...");
      Menus::openMenuSelection();
    }
    return;
  }
  // otherwise just play the modes
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
  // clear all the leds
  Leds::clearAll();
  Leds::update();
#ifdef VORTEX_ARDUINO
  // Set LED data pin (PA7) to input with pull-up resistor
  //PORTA.DIRCLR = PIN7_bm;
  //PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
  //// Set Li-Fi pin (PC5) to input with pull-up resistor
  //PORTC.DIRCLR = PIN5_bm;
  //PORTC.PIN5CTRL = PORT_PULLUPEN_bm;
  // Set wake interrupt on both edges
  PORTB.PIN2CTRL = 0x1;
  // close gate to mosfet to cut power to peripherals
  //PORTB.OUTCLR = PIN0_bm;
  // Set sleep mode to POWER DOWN mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // Enable sleep mode, but not going to sleep yet
  sleep_enable();
  // enter sleep
  sleep_cpu();
#else
  INFO_LOG("SLEEPING");
#endif
}

#ifdef VORTEX_ARDUINO
// interrupt handler to wakeup device on button press
ISR(PORTB_PORT_vect)
{
  if (!(PORTB.INTFLAGS & (1 << 2))) {
    // don't trigger unless it was from the button press
    return;
  }
  // handled
  PORTB.INTFLAGS = (1 << 2);
  // turn off interrupt
  PORTB.PIN2CTRL &= ~PORT_ISC_gm;
  // turn the LED mosfet back on
  //PORTB.OUTSET = PIN0_bm;
  // just reset
  _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
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
