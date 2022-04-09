#include "Buttons.h"

Buttons::Buttons() {
}

void Buttons::createButton(int pin){
  pinNum = pin;
  pinMode(pinNum, INPUT_PULLUP);
}
