#ifndef MODE_H
#define MODE_H

#include "LedConfig.h"

class Pattern;
class Colorset;
class LedControl;
class TimeControl;

class Mode
{
  public:
    Mode();

    // bind a pattern and colorset to individual LED
    bool bind(LedPos pos, Pattern *pat, Colorset *set);
    // bind a pattern and colorset to a range of LEDs
    bool bindRange(LedPos first, LedPos last, Pattern *pat, Colorset *set);
    // bind a pattern and colorset to all LEDs
    bool bindAll(Pattern *pat, Colorset *set);

    // Get and set colorsets
    Pattern *getPattern(LedPos pos);
    Colorset *getColorset(LedPos pos);

    // Play the mode
    void play(const TimeControl *timeControl, LedControl *ledControl);

  private:
    // An array of patterns, on for each LED
    Pattern *m_pPatterns[LED_COUNT];
    // An array of Colorsets, one for each LED
    Colorset *m_pColorsets[LED_COUNT];
};

#endif
