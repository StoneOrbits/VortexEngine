#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "Menu.h"

class Mode;

class Randomizer : public Menu
{
  public:
    Randomizer();

    bool run(const TimeControl *timeControl, const Button *button, LedControl *ledControl);

  private:
    // a pointer to a randomized mode
    Mode *m_pRandomizedMode;

    // re-roll a new randomizatiok
    bool reRoll();
};

#endif
