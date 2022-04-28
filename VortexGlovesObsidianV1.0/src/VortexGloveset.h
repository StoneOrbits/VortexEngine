#ifndef VORTEX_GLOVESET_H
#define VORTEX_GLOVESET_H

// TODO STILL:
/*
 *   - Modes saving loading
 *   - Mode Sharing
 *   - Color selct menu
 *   - Pattern select menu
 *   - Smart randomizer
 *   - Implement missing patterns
 *   - Debug/error logging
 */

class VortexGloveset
{
  // private unimplemented constructor
  VortexGloveset();

public:
  // setup and initialization function
  static bool init();
  // tick function for each loop
  static void tick();

private:

};

#endif
