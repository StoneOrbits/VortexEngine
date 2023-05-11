#include "Button.h"

#include <Arduino.h>

#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

Button::Button() :
  m_pinNum(0),
  m_pressTime(0),
  m_releaseTime(0),
  m_holdDuration(0),
  m_releaseDuration(0),
  m_buttonState(false),
  m_newPress(false),
  m_newRelease(false),
  m_isPressed(false),
  m_shortClick(false),
  m_longClick(false)
{
}

Button::~Button()
{
}

bool Button::init(int pin)
{
  m_pinNum = 0;
  m_pressTime = 0;
  m_releaseTime = 0;
  m_holdDuration = 0;
  m_releaseDuration = 0;
  m_buttonState = false;
  m_newPress = false;
  m_newRelease = false;
  m_isPressed = false;
  m_shortClick = false;
  m_longClick = false;

  m_pinNum = pin;
  pinMode(m_pinNum, INPUT_PULLUP);
  return true;
}

void Button::check()
{
  // reset the new press/release members this tick
  m_newPress = false;
  m_newRelease = false;

  // read the new button state, 0 (LOW) means pressed
  bool newButtonState = (digitalRead(m_pinNum) == 0);

  // did the button change (press/release occurred)
  if (newButtonState != m_buttonState) {
    // set the new state
    m_buttonState = newButtonState;
    // update the currently pressed member
    m_isPressed = m_buttonState;

    // update the press/release times and newpress/newrelease members
    if (m_isPressed) {
      // the button was just pressed
      m_pressTime = Time::getCurtime();
      m_newPress = true;
    } else {
      // the button was just released
      m_releaseTime = Time::getCurtime();
      m_newRelease = true;
    }
  }

  // calculate new hold/release durations if currently held/released
  if (m_isPressed) {
    // update the hold duration as long as the button is pressed
    if (Time::getCurtime() >= m_pressTime) {
      m_holdDuration = (uint32_t)(Time::getCurtime() - m_pressTime);
    }
  } else {
    // update the release duration as long as the button is released
    if (Time::getCurtime() >= m_releaseTime) {
      m_releaseDuration = (uint32_t)(Time::getCurtime() - m_releaseTime);
    }
  }

  // whether a shortclick or long click just occurred
  m_shortClick = (m_newRelease && (m_holdDuration <= SHORT_CLICK_THRESHOLD_TICKS));
  m_longClick = (m_newRelease && (m_holdDuration > SHORT_CLICK_THRESHOLD_TICKS));

  if (m_shortClick) {
    DEBUG_LOG("Short click");
  }
  if (m_longClick) {
    DEBUG_LOG("Long click");
  }
}
