#include "Button.h"
#include "Time.h"

Button::Button() :
  pinNum(0),
  buttonState(HIGH),
  pressTime(0),
  releaseTime(0)
{
}

void Button::init(int pin)
{
  pinNum = pin;
  pinMode(pinNum, INPUT_PULLUP);
}

void Button::check()
{
  // read the new button state
  int newButtonState = digitalRead(pinNum);

  // nothing changed
  if (newButtonState == buttonState) {
    return;
  }

  // set the new state
  buttonState = newButtonState;

  if (buttonState == LOW) {
    // the button was just pressed
    pressTime = g_curTime;
  } else if (buttonState == HIGH) {
    // the button was just released
    releaseTime = g_curTime;
  }
}

// whether the button was pressed this tick
bool Button::on_press()
{
  return (g_curTime == pressTime);
}

// whether the button was released this tick
bool Button::on_release()
{
  return (g_curTime == releaseTime);
}

// whether the button is currently pressed
bool Button::is_pressed()
{
  return (buttonState == LOW);
}

// whether the button is currently released
bool Button::is_released()
{
  return (buttonState == HIGH);
}

// how long the button is currently or was last held down
int Button::holdDuration()
{
  // if the button is actively pressed right now then return how long it's been
  // pressed, otherwise if the button is no longer pressed then return how long 
  // it was held up till it's last release time
  if (is_pressed()) {
    if (g_curTime < pressTime) {
      // this should be impossible but just in case
      return 0;
    }
    // return how long it's been pressed
    return g_curTime - pressTime;
  }
  if (releaseTime < pressTime) {
    // this should also be impossible but just in case
    return 0;
  }
  // return how long it was last held for
  return releaseTime - pressTime;
}

// how long the button is currently or was last released for
int Button::releaseDuration()
{
  // if the button is actively released right now then return how long it's 
  // been released, otherwise if the button is pressed then return how long 
  // it was released till it's last press time
  if (is_released()) {
    if (g_curTime < releaseTime) {
      // this should be impossible but just in case
      return 0;
    }
    // return how long it's been pressed
    return g_curTime - releaseTime;
  }
  if (pressTime < releaseTime) {
    // this should also be impossible but just in case
    return 0;
  }
  // return how long it was last released for
  return pressTime - releaseTime;
}
