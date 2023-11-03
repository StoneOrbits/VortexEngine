#include "src/VortexEngine.h"

int main()
{
  VortexEngine::init();
  while (1) {
    VortexEngine::tick();
  }
  return 0;
}
