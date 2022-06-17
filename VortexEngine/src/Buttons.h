#ifndef BUTTONS_H
#define BUTTONS_H

#include "Button.h"

class Buttons
{
  // private unimplemented constructor
  Buttons();

public:
  // initialize all buttons
  static bool init();
  static void cleanup();

  // poll the buttons
  static void check();

private:
  // feel free to add more I guess
  static Button m_button;
};

// best way I think
extern Button *g_pButton;

#endif
