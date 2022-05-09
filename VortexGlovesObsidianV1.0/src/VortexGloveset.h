#ifndef VORTEX_GLOVESET_H
#define VORTEX_GLOVESET_H

// TODO STILL:
/*
 *  Primary:
 *   - Implement one complex mode
 *   - finish Color selct menu
 *   - finish Pattern select menu
 *   - Modes saving loading
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
