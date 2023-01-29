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

#ifdef TEST_FRAMEWORK
  // so the pattern strip will draw the right mode
  virtual Mode *curMode() const { return m_pRandomizedMode; }
#endif

private:
  // a pointer to a randomized mode
  Mode *m_pRandomizedMode;

  // re-roll a new randomization
  bool reRoll();
};

#endif
