#ifndef VORTEX_GLOVESET_H
#define VORTEX_GLOVESET_H

#include "Button.h"

class VortexGloveset
{
  private:
    // private copy and assignment constructors to prevent copies
    VortexGlovset(VortexGlovset const&) {}
    void operator=(VortexGlovset const&) {}

  public:
    // constructor
    VortexGloveset();

    // setup and initialization function
    bool init();

    // tick function for each loop
    void tick();

  private:
    // the button on the gloves
    Button button;

    // setup serial communications
    bool setupSerial();

#if 0
    void setDefaults();
    void importMode(const char input[]);
    void importValues(const char input[]);
#endif

};

#endif
