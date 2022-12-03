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

private:
  // override showExit so it isn't displayed on thumb
  virtual void showExit() override;

  // a pointer to a randomized mode
  Mode *m_pRandomizedMode;

  // re-roll a new randomizatiok
  bool reRoll();
};

#endif
