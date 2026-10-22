#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>
#include "Button.h"

// the number of buttons, this isn't in VortexConfig.h because
// changing it won't really work without updating other things
// like which pins the buttons are attached to. So this is more
// of a hardcoded constant than a configuration setting
#define NUM_BUTTONS 1

// initialize all buttons
bool Buttons_init(void);
// release the button resources
void Buttons_cleanup(void);
// poll the buttons
void Buttons_update(void);
// the number of buttons
uint8_t Buttons_numButtons(void);

// best way I think
extern Button *g_pButton;

#endif
