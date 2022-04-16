#ifndef MODE_H
#define MODE_H

#include "Arduino.h"

class Pattern;
class Colorset;

class Mode
{
  public:
    Mode();
    Mode(Pattern *pat, Colorset *set);

    // Play the mode
    void play();

  private:
    // A mode consists of a link to a Pattern and a Colorset
    Pattern *m_pPattern;
    Colorset *m_pColorset;
};

#endif
