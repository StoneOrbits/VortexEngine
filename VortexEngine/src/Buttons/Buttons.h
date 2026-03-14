#ifndef BUTTONS_H
#define BUTTONS_H

#include "Button.h"

// the number of buttons, this isn't in VortexConfig.h because
// changing it won't really work without updating other things
// like which pins the buttons are attached to. So this is more
// of a hardcoded constant than a configuration setting
#define NUM_BUTTONS 1

#define BUTTON_PIN 9

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
  static uint8_t m_cancelInterruptPin;
};

// best way I think
extern Button *g_pButton;

#endif
