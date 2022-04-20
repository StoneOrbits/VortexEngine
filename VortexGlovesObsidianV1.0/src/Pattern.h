#ifndef PATTERN_H
#define PATTERN_H

class Colorset;
class LedControl;

class Pattern
{
  public:
    Pattern();
    virtual ~Pattern();

    // pure virtual must override the play function
    virtual void play(LedControl *ledControl, Colorset *colorset) = 0;

  private:
};

#endif
