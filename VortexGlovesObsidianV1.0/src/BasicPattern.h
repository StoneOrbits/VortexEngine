#ifndef BASIC_PATTERN_H
#define BASIC_PATTERN_H

#include "Pattern.h"

class BasicPattern
{
  public:
    BasicPattern(uint32_t onDuration, uint32_t offDuration = 0);

    // must have a bind func
    bool bind(const Colorset *colorset);
    
    // pure virtual must override the play function
    void play(LedControl *ledControl);

  private:
    // =================================================
    //  variables that are initialized with the pattern

    // durations
    uint32_t m_onDuration;
    uint32_t m_offDuration;
    
    // the time for a single on + off
    uint32_t m_blinkDuration;

    // =================================================
    //  variables that change with colorset bind

    // the total duration of the entire pattern with N colors
    uint32_t m_totalDuration;

    // =================================================
    //  variables that will change at runtime

    // the state of the light 
    bool m_lightIsOn;
};

#endif
