#include "Pattern.h"

#include "../LedControl.h"
#include "../Colorset.h"

Pattern::Pattern() 
{
}

Pattern::~Pattern()
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
