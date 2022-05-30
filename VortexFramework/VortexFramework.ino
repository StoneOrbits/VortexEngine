#include <Arduino.h>

#include "src/VortexFramework.h"

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
