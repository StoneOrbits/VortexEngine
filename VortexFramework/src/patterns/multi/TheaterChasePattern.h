#ifndef THEATER_CHASE_PATTERN_H
#define THEATER_CHASE_PATTERN_H

#include "MultiLedPattern.h"

// Rabbit is PATTERN_STROBIE on tops and PATTERN_STROBE on tips
class TheaterChasePattern : public MultiLedPattern
{
public:
  TheaterChasePattern();
  virtual ~TheaterChasePattern();

  // init the pattern to initial state
  virtual void init();

  // pure virtual must override the play function
  virtual void play();

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer &buffer) const;
  virtual void unserialize(SerialBuffer &buffer);

private:

  bool m_oneHundy;
  bool m_twentyThree;
};

#endif
