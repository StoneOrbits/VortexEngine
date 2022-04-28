#ifndef BUTTONS_H
#define BUTTONS_H

#include "Button.h"

class Buttons
{
  // private unimplemented constructor
  Buttons();

public:
  static bool init();

private:
  // feel free to add more I guess
  static Button m_button;
};

// best way I think
extern Button *g_pButton;

#endif
