#ifndef BASIC_PATTERN_H
#define BASIC_PATTERN_H

#include <inttypes.h>

#include "Pattern.h"

class BasicPattern : public Pattern
{
  public:
    BasicPattern(uint32_t onDuration, uint32_t offDuration = 0);

    // pure virtual must override the play function
    void play(LedControl *ledControl, Colorset *colorset);

  private:
    // =================================================
    //  variables that are initialized with the pattern

    // durations
    uint32_t m_onDuration;
    uint32_t m_offDuration;
    
    // the time for a single on + off
    uint32_t m_blinkDuration;

    // =================================================
    //  variables that will change at runtime

    // the current color
    uint32_t m_colorIndex;

    // the state of the light 
    bool m_lightIsOn;
};

#endif
