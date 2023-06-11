#include "src/VortexEngine.h"

int main()
{
  VortexEngine::init();
  for (;;) {
    VortexEngine::tick();
  }
  return 0;
}
