#include "Buttons.h"

// Since there is only one button I am just going to expose a global pointer to 
// access it, instead of making the Button class static in case a second button 
// is added. This makes it easier to access the button from other places while 
// still allowing for a second instance to be added.  I wish there was a more 
// elegant way to make the button accessible but not global.  
// This will simply point at Buttons::m_button. 
Button *g_pButton = nullptr;

// static members
Button Buttons::m_button;

bool Buttons::init()
{
  // initialize the button on pin 1
  if (!m_button.init(1)) {
    return false;
  }
  g_pButton = &m_button;
  return true;
}

void Buttons::check()
{
  // would iterate all buttons and check them here
  // but there's only one button so
  g_pButton->check();
}
