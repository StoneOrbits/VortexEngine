#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "../Menu.h"

#include "../../Random/Random.h"
#include "../../Modes/Mode.h"

class Mode;

class Randomizer : public Menu
{
public:
  Randomizer(const RGBColor &col, bool advanced);
  ~Randomizer();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
  // random context for each led and led multi (LED_COUNT + 1)
  Random m_singlesRandCtx[LED_COUNT];
  Random m_multiRandCtx;

  // the time of the last randomization
  uint32_t m_lastRandomization;

  enum RandomizeFlags : uint8_t {
    // this isn't a valid randomization state, if the flags are
    // on this state then the user will be prompted to pick
    RANDOMIZE_NONE = 0,

    // the two main kinds of randomization
    RANDOMIZE_COLORSET = (1 << 0),
    RANDOMIZE_PATTERN = (1 << 1),

    // an additional optional flag that can be toggled
    RANDOMIZE_AUTO_CYCLE = (1 << 2),

    // an auxilliary flag used to manage whether the user
    // has selected a valid randomization state yet
    RANDOMIZE_SELECTION_COMPLETE = (1 << 3),

    // compound flags both colorset and pattern randomization
    RANDOMIZE_BOTH = (RANDOMIZE_COLORSET | RANDOMIZE_PATTERN)
  };

  // the randomization flags above
  uint8_t m_flags;

  // show the randomization type selection
  void showRandomizationSelect();

  // control the auto cycling
  bool autoCycle() { return (m_flags & RANDOMIZE_AUTO_CYCLE) != 0; }
  void toggleAutoCycle() { m_flags = (RandomizeFlags)((uint8_t)m_flags ^ RANDOMIZE_AUTO_CYCLE); }

  // generate a random colorset with a random context
  Colorset rollColorset(Random &ctx);
  PatternID rollPattern(Random &ctx);

  // re-roll a new randomization with a given context on an led
  bool reRoll(LedPos pos);
  bool reRoll();
};

#endif
