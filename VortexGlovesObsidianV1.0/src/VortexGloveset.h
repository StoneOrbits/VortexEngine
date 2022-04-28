#ifndef VORTEX_GLOVESET_H
#define VORTEX_GLOVESET_H

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
  // ==================
  //  private routines

  // setup routines
  static bool setupSerial();
  // run the menu logic, return false if nothing to do
  static bool runAllMenus();
  // run the current mode
  static void playMode();
};

#endif
