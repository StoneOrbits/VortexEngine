#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "../Menu.h"

class Mode;

class Randomizer : public Menu
{
public:
  Randomizer();
  ~Randomizer();

  bool init();

  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();
  void onLongClick2();

private:
  // a pointer to a randomized mode
  Mode *m_pRandomizedMode;

  // re-roll a new randomization
  bool reRoll();
};

#endif
