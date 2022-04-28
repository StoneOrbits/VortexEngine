#include <Arduino.h>

#include "src/VortexGloveset.h"

void setup()
{
  if (!VortexGloveset::init()) {
    // uhoh
  }
}

void loop()
{
  VortexGloveset::tick();
}
