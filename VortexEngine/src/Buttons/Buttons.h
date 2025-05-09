#ifndef BUTTONS_H
#define BUTTONS_H

#include "Button.h"

#ifdef VORTEX_EMBEDDED
#include <Arduino.h>
#endif

// the number of buttons, this isn't in VortexConfig.h because
// changing it won't really work without updating other things
// like which pins the buttons are attached to. So this is more
// of a hardcoded constant than a configuration setting
#define NUM_BUTTONS 3

#define BUTTON_L_PIN 5
#define BUTTON_M_PIN 6
#define BUTTON_R_PIN 7

class Buttons
{
  // private unimplemented constructor
  Buttons();

public:
  // initialize all buttons
  static bool init();
  static void cleanup();

  // poll the buttons
  static void update();

  static uint8_t numButtons() { return NUM_BUTTONS; }

  // This is a special feature mainly for cancelling the VLSender by installing
  // an interrupt on the button pin in order to detect the press in realtime
  static void installCancelInterrupt(uint8_t pin);
  static void removeCancelInterrupt();
  static bool isCancelRequested();

private:
  // feel free to add more I guess
  static Button m_buttons[NUM_BUTTONS];

#ifdef VORTEX_EMBEDDED
  static void IRAM_ATTR cancelButtonISR();
#endif
  static uint8_t m_cancelInterruptPin;
  static volatile bool cancelButtonPressed;
};

// Button Left
extern Button *g_pButtonL;
// Button Mid
extern Button *g_pButtonM;
// Button Right
extern Button *g_pButtonR;

#endif
