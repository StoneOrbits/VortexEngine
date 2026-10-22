#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <inttypes.h>

#include "../VortexConfig.h"

// although there is only one button on the VortexFramework
// I am still opting for a non-static button struct
//
// the members are intentionally left public so the VortexLib side can
// manipulate them directly to inject button events
typedef struct Button {
  // ========================================
  // state data that is populated each check

  // the timestamp of when the button was pressed
  uint32_t m_pressTime;
  // the timestamp of when the button was released
  uint32_t m_releaseTime;

  // the last hold duration
  uint32_t m_holdDuration;
  // the last release duration
  uint32_t m_releaseDuration;

  // the number of repeated presses (automatically detects rapid presses)
  uint8_t m_consecutivePresses;
  // the number of times released, will overflow at 255
  uint8_t m_releaseCount;

  // the active state of the button
  bool m_buttonState;

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
} Button;

// initialize a new button object, this does not touch the pin
void Button_make(Button *self);
void Button_destroy(Button *self);

// initialize a new button object with a pin number
bool Button_init(Button *self, uint8_t pin);
// directly poll the pin for whether it's pressed right now
bool Button_check(Button *self);
// poll the button pin and update the state of the button object
void Button_update(Button *self);

#ifdef VORTEX_EMBEDDED
// enable the button-wake trigger to wake the device on press
void Button_enableWake(Button *self);
#endif

// whether the button was pressed this tick
bool Button_onPress(const Button *self);
// whether the button was released this tick
bool Button_onRelease(const Button *self);
// whether the button is currently pressed
bool Button_isPressed(const Button *self);

// whether the button was shortclicked this tick
bool Button_onShortClick(const Button *self);
// whether the button was long clicked this tick
bool Button_onLongClick(const Button *self);
// fired when a certain number of presses is reached, the consecutive press
// counter is automatically reset when that happens
bool Button_onConsecutivePresses(Button *self, uint8_t numPresses);

// when the button was last pressed
uint32_t Button_pressTime(const Button *self);
// when the button was last released
uint32_t Button_releaseTime(const Button *self);

// how long the button is currently or was last held down (in ticks)
uint32_t Button_holdDuration(const Button *self);
// how long the button is currently or was last released for (in ticks)
uint32_t Button_releaseDuration(const Button *self);

// the number of consecutive presses
uint8_t Button_consecutivePresses(const Button *self);
// the number of releases
uint8_t Button_releaseCount(const Button *self);

// See Button.cpp for info about this
extern Button *g_pButton;

#endif
