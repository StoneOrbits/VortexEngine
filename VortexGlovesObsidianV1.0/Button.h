#ifndef BUTTON_H
#define BUTTON_H

#include "Arduino.h"


class Button 
{
  public:
    Button();

    void init(int pin);

    int pinNum;
    int buttonState;
    int lastButtonState;
    unsigned long pressTime;
    unsigned long prevPressTime;
    unsigned long holdTime;
    unsigned long prevHoldTime;
};

#endif
