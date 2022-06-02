#ifndef VORTEX_FRAMEWORK_H
#define VORTEX_FRAMEWORK_H

// TODO STILL:
/*
 *  Everything is working somewhat
 *
 *  Secondary:
 *   - Implement missing patterns
 *   - Smart randomizer
 *   - Improve Mode Sharing logic/code/approach
 *
 *  Other:
 *   - Demo and demo speed?
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
