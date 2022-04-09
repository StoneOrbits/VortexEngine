#include "Button.h"

Button::Button() 
{
}

void Button::init(int pin)
{
  pinNum = pin;
  pinMode(pinNum, INPUT_PULLUP);
}
