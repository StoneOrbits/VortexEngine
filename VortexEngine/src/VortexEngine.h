#ifndef VORTEX_ENGINE_H
#define VORTEX_ENGINE_H

// TODO STILL:
/*
 *  Everything is working somewhat well
 *
 *  But there is still more to work on:
 *   - Randomize patterns?
 *   - Implement missing patterns
 *   - IR transmission protocol to allow any length data instead of single burst
 *   - Saving modes that have custom parameters won't work
 *      - conslusion:
 *          Will save parameters to serial, at risk of causing misalignments if people remove
 *          patterns or adjust the number of parameters a pattern takes. This is just part of
 *          the risk of editing the firmware.
 *          
 *          Needs ir improvements first so that added data size can still be transmitted
 *
 *   - palm light?
 *   - Code polishing and final build solidification
 *   - Code documentation
 *   - eliminate library code to reduce size
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
