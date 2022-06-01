#ifndef VORTEX_FRAMEWORK_H
#define VORTEX_FRAMEWORK_H

// TODO STILL:
/*
 *  Primary:
 *   - Mode Sharing
 *
 *  Secondary:
 *   - Smart randomizer
 *   - Implement missing patterns
 *
 *  Other:
 *   - Demo and demo speed?
 *   - Debug/error logging
 */

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
