#include <Arduino.h>
#include "src/Serial/Serial.h"
#include "src/VortexEngine.h"

void setup()
{
  if (!VortexEngine::init()) {
    // uhoh
  }
}

void loop()
{
  VortexEngine::tick();
}
