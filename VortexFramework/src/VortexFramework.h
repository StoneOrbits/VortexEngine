#ifndef VORTEX_FRAMEWORK_H
#define VORTEX_FRAMEWORK_H

// TODO STILL:
/*
 *  Everything is working somewhat well
 *
 *  But there is still more to work on:
 *   - Implement missing patterns
 *   - Smart randomizer
 *   - Improve Mode Sharing logic/code/approach
 *   - Code documentation
 */

#define VORTEX_VERSION "0.1 alpha"

class VortexFramework
{
  // private unimplemented constructor
  VortexFramework();

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
