#ifndef VORTEX_ENGINE_H
#define VORTEX_ENGINE_H

// TODO STILL:
/*
 *  Everything is working somewhat well
 *
 *  But there is still more to work on:
 *   - Fix bug in compression/decompression chopping last byte (blue)
 *   - eliminate library code to reduce size
 *   - palm light?
 *   - refactor color constants (add flag to indicate RGB/HSV)
 *   - Code polishing and final build solidification
 *   - Code documentation (DOXYGEN???)
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
};

#endif
