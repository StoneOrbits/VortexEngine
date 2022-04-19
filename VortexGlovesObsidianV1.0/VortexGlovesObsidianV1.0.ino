#include <Arduino.h>

#include "src/VortexGloveset.h"

VortexGloveset gloveset;

void setup()
{
  if (!gloveset.init()) {
    // uhoh
  }
}

void loop()
{
  gloveset.tick();
}
