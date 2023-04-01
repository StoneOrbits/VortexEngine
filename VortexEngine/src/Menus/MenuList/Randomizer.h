#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "../Menu.h"
#include "../../Modes/Mode.h"

class Mode;

class Randomizer : public Menu
{
public:
  Randomizer();
  ~Randomizer();

  bool init() override;

  bool run() override;

  // handlers for clicks
  //void onShortClick() override;
  //void onLongClick() override;

private:
};

#endif
