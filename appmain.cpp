#include "VortexEngine.h"

#include <Arduino.h>

int main()
{
  onBeforeInit(); // Emnpty callback called before init but after the .init stuff. First normal code executed
  init(); // Interrupts are turned on just prior to init() returning.
  initVariant();
  if (!onAfterInit()) sei();  // enable interrupts.
  VortexEngine::init();
  for (;;) {
    VortexEngine::tick();
  }
	return 0;
}
