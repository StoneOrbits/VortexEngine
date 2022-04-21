#include "Button.h"

#include "TimeControl.h"

// if click held for <= this value then the click will be registered as 
// a 'short click' otherwise if held longer than this threshold it will
// be registered as a 'medium click'
//
// There is no need to distinguish a 'medium click' from a 'long hold'.
// A long hold will only ever be available when 'medium click' is not.
//
// The long hold is detected by just checking the holdDuration()
#define SHORT_CLICK_THRESHOLD 50

Button::Button() :
  m_pinNum(0),
  m_buttonState(HIGH),
  m_pressTime(0),
  m_releaseTime(0),
  m_holdDuration(0),
  m_releaseDuration(0),
  m_newPress(false),
  m_newRelease(false),
  m_isPressed(false),
  m_shortClick(false),
  m_mediumClick(false)
{
}

bool Button::init(int pin)
{
  m_pinNum = pin;
  pinMode(m_pinNum, INPUT_PULLUP);
  return true;
}

void Button::check()
{
  // reset the new press/release members this tick
  m_newPress = false;
  m_newRelease = false;

  // read the new button state
  int newButtonState = digitalRead(m_pinNum);

  // did the button change (press/release occurred)
  if (newButtonState != m_buttonState) {
    // set the new state
    m_buttonState = newButtonState;
    // update the currently pressed member
    m_isPressed = (m_buttonState == LOW);

    // update the press/release times and newpress/newrelease members
    if (m_buttonState == LOW) {
      // the button was just pressed
      m_pressTime = g_curTime;
      m_newPress = true;
    } else if (m_buttonState == HIGH) {
      // the button was just released
      m_releaseTime = g_curTime;
      m_newRelease = true;
    }
  }

  // calculate new hold/release durations if currently held/released
  if (m_isPressed) {
    // update the hold duration as long as the button is pressed
    if (g_curTime >= m_pressTime && m_pressTime != 0) {
      m_holdDuration = g_curTime - m_pressTime;
    }
  } else {
    // update the release duration as long as the button is released
    if (g_curTime >= m_releaseTime && m_releaseTime != 0) {
      m_releaseDuration = g_curTime - m_releaseTime;
    }
  }

  // whether a shortclick or medium click just occurred
  m_shortClick = (m_newRelease && (m_holdDuration <= SHORT_CLICK_THRESHOLD));
  m_mediumClick = (m_newRelease && (m_holdDuration > SHORT_CLICK_THRESHOLD));
}
