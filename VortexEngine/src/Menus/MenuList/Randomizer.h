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

  // re-roll a new randomization with a given context on an led
  bool reRoll();

private:
  // internal helper functions
  void showRandomizationSelect();
  bool reRollMulti();
  bool reRollSingles();
  bool reRollForContext(Random &ctx, LedPos pos);
  bool splitMultiRandomize();
  bool applyPatternAndColorsetToMap(LedMap map, PatternID pattern, const Colorset &colorset);
  PatternID rollSingleLedPatternID(Random &ctx);
  PatternID rollMultiLedPatternID(Random &ctx);
  Colorset rollColorset(Random &ctx);
  bool rollCustomPattern(Random &ctx, Mode *pMode, LedPos pos);
  void traditionalPattern(Random &ctx, PatternArgs &outArgs);
  void gapPattern(Random &ctx, PatternArgs &outArgs);
  void dashPattern(Random &ctx, PatternArgs &outArgs);
  void crushPattern(Random &ctx, PatternArgs &outArgs);

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

    // compound flags both colorset and pattern randomization
    RANDOMIZE_BOTH = (RANDOMIZE_COLORSET | RANDOMIZE_PATTERN)
  };

  // the randomization flags above
  uint8_t m_flags;

  // auxilliary variable to display cycling hue at menus
  uint8_t m_displayHue;

  // whether still need to select a randomization type
  bool m_needToSelect;
  // whether auto cycling
  bool m_autoCycle;
};

#endif
