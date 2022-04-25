#ifndef TRACER_PATTERN_H
#define TRACER_PATTERN_H

#include "Pattern.h"

class TracerPattern : public Pattern
{
  public:
    TracerPattern(uint32_t tracerLength, uint32_t dotLength);

    // pure virtual must override the play function
    void play(Colorset *colorset, LedPos pos);

    // must override the serialize routine to save the pattern
    void serialize() const;
    // must override unserialize to load patterns
    void unserialize();

  private:

    uint32_t m_tracerDuration;
    uint32_t m_totalDuration;
    uint32_t m_tracerCounter;

    // the state of the light (only two states)
    bool m_state;
};

#endif
