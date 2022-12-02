#ifndef VORTEX_ENGINE_H
#define VORTEX_ENGINE_H

// TODO STILL:
/*
 *  Everything is working!
 *
 *  But there is still more to work on:
 *   - eliminate library code to reduce size
 *   - palm light?
 *   - Code documentation (DOXYGEN???)
 *   - Color select ring menu color orange is off
 *   - in color select:
 *      - when no colors left thumb stays red pinkie blinks green
 *      - enhance deletion by blinking tip red/white when selecting color
 */

#include "VortexConfig.h"

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

private:
#if COMPRESSION_TEST == 1
  static void compressionTest();
#endif
#if SERIALIZATION_TEST == 1
  static void serializationTest();
#endif
};

#endif
