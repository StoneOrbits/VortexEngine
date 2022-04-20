#ifndef MODE_H
#define MODE_H

class Pattern;
class Colorset;
class LedControl;

class Mode
{
  public:
    Mode();
    Mode(Pattern *pat, Colorset *set);

    Pattern *getPattern() { return m_pPattern; }
    void setPattern(Pattern *pat) { m_pPattern = pat; }

    Colorset *getColorset() { return m_pColorset; }
    void setColorset(Colorset *set) { m_pColorset = set; }

    // Play the mode
    void play(LedControl *ledControl);

  private:
    // A mode consists of a link to a Pattern and a Colorset
    Pattern *m_pPattern;
    Colorset *m_pColorset;
};

#endif
