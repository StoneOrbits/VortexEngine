#ifndef DEMO_PATTERN_H
#define DEMO_PATTERN_H

#include "Pattern.h"

class DemoPattern : public Pattern
{
  public:
    DemoPattern();

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
};



#endif
