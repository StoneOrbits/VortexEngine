#ifndef PATTERN_H
#define PATTERN_H

class Colorset;
class LedControl;

class Pattern
{
  public:
    Pattern();
    virtual ~Pattern();

    // Must bind a pattern with a colorset before it can play
    virtual bool bind(const Colorset *colorset);
    
    // pure virtual must override the play function
    virtual void play(LedControl *ledControl) = 0;

  private:
    // the connected colorset
    const Colorset *m_pColorset;
};

#endif
