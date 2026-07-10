#ifndef BUTTON_H
#define BUTTON_H

#include <inttypes.h>
#include <stdbool.h>
#include "../VortexConfig.h"

typedef struct Button_s {
  uint32_t pressTime;
  uint32_t releaseTime;
  uint32_t holdDuration;
  uint32_t releaseDuration;
  uint8_t consecutivePresses;
  uint8_t releaseCount;
  bool buttonState;
  bool newPress;
  bool newRelease;
  bool isPressed;
  bool shortClick;
  bool longClick;
} Button;

void Button_init(Button *self);
void Button_cleanup(Button *self);
bool Button_initPin(Button *self, uint8_t pin);
bool Button_check(Button *self);
void Button_update(Button *self);
#ifdef VORTEX_EMBEDDED
void Button_enableWake(Button *self);
#endif
bool Button_onPress(const Button *self);
bool Button_onRelease(const Button *self);
bool Button_isPressed(const Button *self);
bool Button_onShortClick(const Button *self);
bool Button_onLongClick(const Button *self);
bool Button_onConsecutivePresses(Button *self, uint8_t numPresses);
uint32_t Button_pressTime(const Button *self);
uint32_t Button_releaseTime(const Button *self);
uint32_t Button_holdDuration(const Button *self);
uint32_t Button_releaseDuration(const Button *self);
uint8_t Button_consecutivePresses(const Button *self);
uint8_t Button_releaseCount(const Button *self);

extern Button *g_pButton;

#endif
