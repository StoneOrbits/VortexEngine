#ifndef BUTTON_H
#define BUTTON_H

#include <inttypes.h>

// although there is only one button on the VortexFramework
// I am still opting for a non-static button class 
class Button
{
private:
  // private unimplemented copy and assignment constructors to prevent copies
  Button(Button const &);
  void operator=(Button const &);

public:
  Button();
  ~Button();

  // initialize a new button object with a pin number
  bool init(int pin);
  // check the state of the button by querying the pin
  void check();

  // whether the button was pressed this tick
  bool onPress() const { return m_newPress; }
  // whether the button was released this tick
  bool onRelease() const { return m_newRelease; }
  // whether the button is currently pressed
  bool isPressed() const { return m_isPressed; }

  // whether the button was shortclicked this tick
  bool onShortClick() const { return m_shortClick; }
  // whether the button was long clicked this tick
  bool onLongClick() const { return m_longClick; }

  // how long the button is currently or was last held down
  uint32_t holdDuration() const { return m_holdDuration; }
  // how long the button is currently or was last released for
  uint32_t releaseDuration() const { return m_releaseDuration; }

private:
  // the pin number that is read
  int m_pinNum;

  // ========================================
  // state data that is populated each check

  // the active state of the button
  uint32_t m_buttonState;

  // the timestamp of when the button was pressed
  uint64_t m_pressTime;
  // the timestamp of when the button was released
  uint64_t m_releaseTime;

  // the last hold duration
  uint32_t m_holdDuration;
  // the last release duration
  uint32_t m_releaseDuration;

  // whether pressed this tick
  bool m_newPress;
  // whether released this tick
  bool m_newRelease;
  // whether currently pressed
  bool m_isPressed;
  // whether a short click occurred
  bool m_shortClick;
  // whether a long click occurred
  bool m_longClick;
};

// See Button.cpp for info about this
extern Button *g_pButton;

#endif
