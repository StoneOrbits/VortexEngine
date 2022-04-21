#ifndef BASIC_PATTERN_H
#define BASIC_PATTERN_H

#include <inttypes.h>

#include "Pattern.h"

class BasicPattern : public Pattern
{
  public:
    BasicPattern(uint32_t onDuration, uint32_t offDuration = 0);
    virtual ~BasicPattern();

    virtual void play(const TimeControl *timeControl, LedControl *ledControl, 
        Colorset *colorset, LedPos pos);

  private:
    // =================================================
    //  variables that are initialized with the pattern

    // how long the light remains on
    uint32_t m_onDuration;
    // the total time for a blink (on duration + off duration)
    uint32_t m_blinkDuration;

    // =================================================
    //  variables that will change at runtime

    // the state of the light 
    bool m_lightIsOn;
};

#endif
