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
    bool bind(Pattern *pat, Colorset *set, LedPos pos = LED_FIRST);
    // bind a pattern and colorset to a range of LEDs
    bool bindRange(Pattern *pat, Colorset *set, LedPos first, LedPos last);
    // bind a pattern and colorset to all LEDs
    bool bindAll(Pattern *pat, Colorset *set);

    // replace just the pattern or colorset
    bool setPattern(Pattern *pat, LedPos pos = LED_FIRST);
    bool setColorset(Colorset *set, LedPos pos = LED_FIRST);

    // Get patterns/colorsets
    Pattern *getPattern(LedPos pos = LED_FIRST);
    Colorset *getColorset(LedPos pos = LED_FIRST);

    // Play the mode
    void play(const TimeControl *timeControl, LedControl *ledControl);

  private:
    // NOTE: Modes *ALLOW* for one pattern and one colorset on each LED
    //       but we are not intending to expose that functionality through
    //       the menus or UI. Instead users will have to customize save
    //       files with separate software to produce custom multi-pattern
    //       or multi-colorset modes
    //
    //       This means in practice all m_pPatterns and m_pColorsets are
    //       separate instances of the same class unless somebody has loaded
    //       a custom savefile
    //
    // An array of patterns, on for each LED
    Pattern *m_pPatterns[LED_COUNT];
    // An array of Colorsets, one for each LED
    Colorset *m_pColorsets[LED_COUNT];
};

#endif
