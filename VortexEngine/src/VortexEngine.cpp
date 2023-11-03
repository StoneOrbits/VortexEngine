#include "VortexEngine.h"

#include "Serial/ByteStream.h"
#include "Storage/Storage.h"
#include "Buttons/Buttons.h"
#include "Time/TimeControl.h"
#include "Time/Timings.h"
#include "Modes/Mode.h"
#include "Leds/Leds.h"
#include "Log/Log.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

Mode g_curMode;

bool VortexEngine::init()
{
  // all of the global controllers
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
  if (!Buttons::init()) {
    DEBUG_LOG("Buttons failed to initialize");
    return false;
  }

  return true;
}

void VortexEngine::cleanup()
{
}

void VortexEngine::tick()
{
  // tick the current time counter forward
  Time::tickClock();

  // poll the button(s) and update the button object states
  Buttons::update();

  // run the main logic for the engine
  runMainLogic();

  // update the leds
  Leds::update();
}

void VortexEngine::runMainLogic()
{
  // the current tick
  uint32_t now = Time::getCurtime();

  g_curMode.play();
}

bool VortexEngine::serializeVersion(ByteStream &stream)
{
  // serialize the vortex version
  return stream.serialize((uint8_t)VORTEX_VERSION_MAJOR) &&
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
