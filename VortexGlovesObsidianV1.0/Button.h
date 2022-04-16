#ifndef BUTTON_H
#define BUTTON_H

#include "Arduino.h"

class Button 
{
  private:
    // private unimplemented copy and assignment constructors to prevent copies
    Button(Button const&);
    void operator=(Button const&);

  public:
    Button();

    // initialize a new button object with a pin number
    bool init(int pin);
    // check the state of the button by querying the pin
    void check();

    // whether the button was pressed this tick
    bool onPress() { return m_newPress; }
    // whether the button was released this tick
    bool onRelease() { return m_newRelease; }
    // whether the button is currently pressed
    bool isPressed() { return m_isPressed; }

    // whether the button was shortclicked this tick
    bool onShortClick() { return m_shortClick; }
    // whether the button was mediumclicked this tick
    bool onMediumClick() { return m_mediumClick; }

    // how long the button is currently or was last held down
    int holdDuration() { return m_holdDuration; }
    // how long the button is currently or was last released for
    int releaseDuration() { return m_releaseDuration; }

  private:
    // the pin number that is read
    int m_pinNum;

    // ========================================
    // state data that is populated each check

    // the active state of the button
    int m_buttonState;

    // the timestamp of when the button was pressed
    unsigned long m_pressTime;
    // the timestamp of when the button was released
    unsigned long m_releaseTime;

    // the last hold duration
    int m_holdDuration;
    // the last release duration
    int m_releaseDuration;

    // whether pressed this tick
    bool m_newPress;
    // whether released this tick
    bool m_newRelease;
    // whether currently pressed
    bool m_isPressed;
    // whether a short click occurred
    bool m_shortClick;
    // whether a medium/long click occurred
    bool m_mediumClick;
};

#endif
