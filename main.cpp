#include <avr/io.h>

#include "VortexEngine.h"

int main(void)
{
  // Initialize the Vortex Engine
  if (!VortexEngine::init()) {
    // uhoh
    return 1;
  }

  // main loop
  while (1) {
    VortexEngine::tick();
  }
  return 0;
}
