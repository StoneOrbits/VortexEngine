#ifndef PATTERN_H
#define PATTERN_H

class Colorset;
class LedControl;

class Pattern
{
  public:
    Pattern();
    virtual ~Pattern();

    // must have an init func
    virtual bool init();
    
    // pure virtual must override the play function
    virtual void play(LedControl *ledControl, const Colorset *colorset);

  private:
};

#endif
