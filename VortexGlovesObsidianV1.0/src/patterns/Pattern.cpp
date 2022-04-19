#include "Pattern.h"
#include "Mode.h"

Pattern::Pattern() 
{
}

bool Pattern::init()
{
  return true;
}


void Pattern::play(LedControl *ledControl, const Colorset *colorset)
{
  // control leds based on colorset and curtime
}
