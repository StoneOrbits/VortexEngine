#include <Arduino.h>

#include "src/VortexFramework.h"

void setup()
{
  if (!VortexFramework::init()) {
    // uhoh
  }
}

void loop()
{
  VortexFramework::tick();
}
