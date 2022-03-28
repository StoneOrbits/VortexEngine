#ifndef Buttons_h
#define Buttons_h

#include "Arduino.h"


class Buttons {

  public:
    Buttons();
    int pinNum;
    int buttonState, lastButtonState;
    unsigned long pressTime, prevPressTime, holdTime, prevHoldTime;
    void createButton(int pin);
  
  private:
  
};

#endif
