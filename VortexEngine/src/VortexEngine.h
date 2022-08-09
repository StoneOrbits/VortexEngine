#ifndef VORTEX_ENGINE_H
#define VORTEX_ENGINE_H

// TODO STILL:
/*
 *  Everything is working somewhat well
 *
 *  But there is still more to work on:
 *   - Fix bug in compression/decompression chopping last byte (blue)
 *   - Move timings of blink on/off to Timings.h
 *   - Randomize patterns?
 *   - Implement missing patterns
 *   - IR transmission protocol to allow any length data instead of single burst
 *   - eliminate library code to reduce size
 *   - palm light?
 *   - Code polishing and final build solidification
 *   - Code documentation
 */

#define VORTEX_VERSION "0.1 alpha"

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
