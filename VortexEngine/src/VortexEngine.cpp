#include "VortexEngine.h"

#include "Serial/ByteStream.h"
#include "Time/TimeControl.h"
#include "Patterns/Pattern.h"
#include "Storage/Storage.h"
#include "Serial/Serial.h"
#include "Time/Timings.h"
#include "Leds/Leds.h"
#include "Log/Log.h"

#include <Arduino.h>

#ifdef VORTEX_ARDUINO
#include <avr/interrupt.h>
#include <avr/sleep.h>
#endif

Mode VortexEngine::m_mode;
Button VortexEngine::m_button;
uint16_t g_sleepCount = 0;

void VortexEngine::enterSleep()
{
  // clear all the leds
  Leds::clearAll();
  Leds::update();
  g_sleepCount = 0;
#ifdef VORTEX_ARDUINO
  // Initialize RTC
  while (RTC.STATUS > 0) {
    // wait till RTC is ready
  }
  // 32.768kHz Internal Ultra-Low-Power Oscillator (OSCULP32K)
  RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
  // PIT Interrupt: enabled
  RTC.PITINTCTRL = RTC_PI_bm;
  // RTC Clock Cycles 16384, resulting in 32.768kHz/16384 = 2Hz
  RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc | RTC_PITEN_bm;
  // enter sleep
  sleep_cpu();
#endif
}

#ifdef VORTEX_ARDUINO
ISR(RTC_PIT_vect)
{
  RTC.PITINTFLAGS = RTC_PI_bm;          /* Clear interrupt flag by writing '1' (required) */
  g_sleepCount++;
  if (g_sleepCount > 3 && digitalRead(5) == LOW) {
    // perform software reset to wake up
    _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
  }
}
#endif

bool VortexEngine::init()
{
  // initialize a random seed
  // Always generate seed before creating button on
  // digital pin 1 (shared pin with analog 0)
  randomSeed(analogRead(0));

  // all of the global controllers
  if (!SerialComs::init()) {
    DEBUG_LOG("Serial failed to initialize");
    return false;
  }
  if (!Time::init()) {
    DEBUG_LOG("Time failed to initialize");
    return false;
  }
  if (!Storage::init()) {
    DEBUG_LOG("Storage failed to initialize");
    return false;
  }
  if (!Leds::init()) {
    DEBUG_LOG("Leds failed to initialize");
    return false;
  }
  if (!m_button.init(5)) {
    DEBUG_LOG("Buttons failed to initialize");
    return false;
  }

  // for access from other files
  g_pButton = &m_button;

  // rgb blend on both
  Colorset set(RGBColor(255, 0, 0), RGBColor(0, 255, 0), RGBColor(0, 0, 255));
  m_mode.setPattern(PATTERN_BLEND);
  m_mode.setColorset(&set);
  m_mode.init();
  //ByteStream data;
  //m_mode.saveToBuffer(data);
  ////data.compress();
  ////data.decompress();
  //m_mode.loadFromBuffer(data);

#if COMPRESSION_TEST == 1
  compressionTest();
#endif

#if SERIALIZATION_TEST == 1
  serializationTest();
#endif
  return true;
}

void VortexEngine::cleanup()
{
}

void VortexEngine::tick()
{
  // tick the current time counter forward
  Time::tickClock();

  m_button.check();

  // check for sleep click
  if (m_button.isPressed() && m_button.holdDuration() >= SLEEP_ENTER_THRESHOLD_TICKS) {
    // enter sleep and return
    enterSleep();
    return;
  }

  // cycle modes?
#if 0
  if (m_button.onShortClick()) {
    PatternArgs args;
    m_mode.getPattern(LED_1)->getArgs(args);
    for (uint32_t i = 0; i < args.numArgs; ++i) {
      args.args[i] = random(30);
    }
    m_mode.getPattern(LED_1)->setArgs(args);
  }
#endif
  if (m_button.onShortClick()) {
    m_mode.init();
  }
  //int val = (m_button.holdDuration() / SHORT_CLICK_THRESHOLD_TICKS);
  //if (val > 8) {
  //  val = 8;
  //}
  //Leds::setAll(hsv_to_rgb_rainbow(HSVColor(val * 30, 255, 255)));
  // then just play the mode
  m_mode.play();
  // update the leds
  Leds::update();
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

#if COMPRESSION_TEST == 1
#include <stdio.h>
#include "Memory/Memory.h"
void VortexEngine::compressionTest()
{
  ByteStream stream;
  for (uint32_t len = 1; len < 4096; ++len) {
    uint8_t *buf = (uint8_t *)vcalloc(1, len + 1);
    if (!buf) {
      continue;
    }
    for (uint32_t i = 0; i < len; ++i) {
      buf[i] = (uint8_t)i&0xFF;
    }
    stream.init(len, buf);
    stream.compress();
    stream.decompress();
    if (memcmp(stream.data(), buf, len) != 0) {
      ERROR_LOGF("Buffers not equal: %u", len);
      printf("\t");
      for (uint32_t i = 0; i < len; ++i) {
        printf("%02x ", buf[i]);
        if (i > 0 && ((i + 1) % 32) == 0) {
          printf("\r\n\t");
        }
      }
      return;
    }
    DEBUG_LOGF("Success %u compressed", len);
    free(buf);
  }
  DEBUG_LOG("Success testing compression");
}
#endif

#if SERIALIZATION_TEST == 1
#include "Modes/Mode.h"
#include "Colors/Colorset.h"
#include "Patterns/Pattern.h"
#include "Patterns/PatternArgs.h"
#include "Patterns/PatternBuilder.h"
#include <stdio.h>
void VortexEngine::serializationTest()
{
  Colorset bigSet;
  for (uint32_t i = 0; i < MAX_COLOR_SLOTS; ++i) {
    bigSet.addColorByHue(i * 31);
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
        printf("%02x ", byte);
        if ((++count % 32) == 0) {
          printf("\n");
        }
      }
      printf("\n");
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
      printf("%02x ", byte);
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
#include "Time/Timer.h"

#include <assert.h>

void VortexEngine::timerTest()
{
  DEBUG_LOG("== Beginning Timer Test ==");
  // run the general time test
  Time::test();
  // run the timer test
  Time::test();
  // if we made it to here then success
  DEBUG_LOG("== Beginning Timer Test ==");
}
#endif

