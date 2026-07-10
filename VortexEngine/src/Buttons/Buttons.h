#ifndef BUTTONS_H
#define BUTTONS_H

#include <inttypes.h>
#include <stdbool.h>
#include "Button.h"
#include "../VortexConfig.h"

#ifdef VORTEX_EMBEDDED
#define REGISTER_BUTTON(pin) Buttons_init(pin)
#else
#define REGISTER_BUTTON(pin) Buttons_init()
#endif

bool Buttons_init(uint8_t pin);
void Buttons_cleanup(void);
void Buttons_update(void);

#endif
