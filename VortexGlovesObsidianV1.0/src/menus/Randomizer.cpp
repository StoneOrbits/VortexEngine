#include "Randomizer.h"

#include "../Button.h"
#include "../LedControl.h"

Randomizer::Randomizer() :
  Menu(HSV_WHITE)
{
}

bool Randomizer::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  if (button->onMediumClick()) {
    // save randomization, exit randomizer menu
    return false;
  }
  if (button->onShortClick()) {
    // re-roll randomization
  }

  // display randomization
  ledControl->setRange(LED_FIRST, LED_LAST, HSV_RED);

  // return true to continue staying in randomizer menu
  return true;
}

