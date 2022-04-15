#ifndef BUTTON_H
#define BUTTON_H

#include "Arduino.h"


class Button 
{
  public:
    Button();

    // initialize a new button object with a pin number
    bool init(int pin);
    // check the state of the button by querying the pin
    void check();

    // whether the button was pressed this tick
    bool on_press();
    // whether the button was released this tick
    bool on_release();
    // whether the button is currently pressed
    bool is_pressed();
    // whether the button is currently released
    bool is_released();

    // how long the button is currently or was last held down
    int holdDuration();
    // how long the button is currently or was last released for
    int releaseDuration();

  private:
    // the pin number that is read
    int pinNum;
    // the active state of the button
    int buttonState;

    // the timestamp of when the button was initially pressed
    unsigned long pressTime;
    // the timestamp of when the button was initially released
    unsigned long releaseTime;
};

#endif
