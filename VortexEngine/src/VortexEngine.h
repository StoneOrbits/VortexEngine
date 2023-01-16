#ifndef VORTEX_ENGINE_H
#define VORTEX_ENGINE_H

#include <inttypes.h>

// TODO STILL:
/*
 *  Everything is working!
 *
 *  But there is still more to work on:
 *   - eliminate library code to reduce size
 *   - palm light?
 *   - Code documentation (DOXYGEN???)
 *   - menu 'blink' api is slightly buggy
 * 
 *   - Editor!
 *   - color wheel/picker!
 *   - color 'mode' (rainbow/generic)
 *   - global brightness slider in editor?
 *   - brightness control PER MODE?
 *   - refactor message pump for editor
 *   - solve issue with editor parsing handshake properly, need to split messages, need delimiter
 *   - 
 */

#include "VortexConfig.h"

class ByteStream;

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

  // serialize the version number to a byte stream
  static void serializeVersion(ByteStream &stream);
  static bool checkVersion(uint8_t major, uint8_t minor);

private:
#if COMPRESSION_TEST == 1
  static void compressionTest();
#endif
#if SERIALIZATION_TEST == 1
  static void serializationTest();
#endif
};

#endif
