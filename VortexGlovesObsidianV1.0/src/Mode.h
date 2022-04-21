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
    Mode(Pattern *pat, Colorset *set);

    // Get and set patterns
    Pattern *getPattern(LedPos pos);
    void setPattern(LedPos pos, Pattern *pat);

    // Get and set colorsets
    Colorset *getColorset(LedPos pos);
    void setColorset(LedPos pos, Colorset *set);

    // Play the mode
    void play(const TimeControl *timeControl, LedControl *ledControl);

  private:
    // An array of patterns, on for each LED
    Pattern *m_pPattern[NUM_LEDS];
    // An array of Colorsets, one for each LED
    Colorset *m_pColorset[NUM_LEDS];
};

#endif
