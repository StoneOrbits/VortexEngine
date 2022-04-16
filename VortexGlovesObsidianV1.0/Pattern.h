#ifndef PATTERN_H
#define PATTERN_H

class Colorset;

class Pattern
{
  public:
    Pattern();

    bool init();
    void play(const Colorset *colorset);

  private:
    //void getColor(int target);
    //void setLed(int target);
    //void setLeds(int first, int last);
    //void nextColor(int start);
    //void nextColor1(int start);
    //void clearAll();
    //void clearLight(int lightNum);
    //void clearLights(int first, int last);
};

#endif
