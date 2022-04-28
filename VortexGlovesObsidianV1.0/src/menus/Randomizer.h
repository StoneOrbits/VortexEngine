#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "Menu.h"

class Mode;

class Randomizer : public Menu
{
public:
  Randomizer();

  bool init();

  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  // a pointer to a randomized mode
  Mode *m_pRandomizedMode;

  // re-roll a new randomizatiok
  bool reRoll();
};

#endif
