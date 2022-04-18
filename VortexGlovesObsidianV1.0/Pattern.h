#ifndef PATTERN_H
#define PATTERN_H

class Colorset;

class Pattern
{
  public:
    Pattern();

    virtual bool init();
    
    // pure virtual must override the play function
    virtual void play(const Colorset *colorset);

  private:
};

#endif
