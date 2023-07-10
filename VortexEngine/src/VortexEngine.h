#ifndef VORTEX_ENGINE_H
#define VORTEX_ENGINE_H

#include <inttypes.h>
#include "VortexConfig.h"

class ByteStream;
class Mode;

class VortexEngine
{
  // private unimplemented constructor
  VortexEngine();

public:
  // setup and initialization function
  static bool init();
  // cleanup
  static void cleanup();

  // tick function for each loop
  static void tick();

  // run the main logic of the engine
  static void runMainLogic();

  // serialize the version number to a byte stream
  static void serializeVersion(ByteStream &stream);
  static bool checkVersion(uint8_t major, uint8_t minor);

  // get the current mode
  static Mode *curMode();

#ifdef VORTEX_LIB
  // the total available storage space
  static uint32_t totalStorageSpace();
  // the size of the savefile
  static uint32_t savefileSize();
  // whether sleeping or not, some versions support sleep
  static bool isSleeping() { return false; }
#endif

private:
#if COMPRESSION_TEST == 1
  static void compressionTest();
#endif
#if SERIALIZATION_TEST == 1
  static void serializationTest();
#endif
#if TIMER_TEST == 1
  static void timerTest();
#endif
};

#endif
