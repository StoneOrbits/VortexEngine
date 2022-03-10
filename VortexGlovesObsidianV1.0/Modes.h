#ifndef Modes_h
#define Modes_h

#include "Arduino.h"


class Modes {
  private:

  public:
    Modes();
    bool endOfSet;
    int hue[8];
    int sat[8];
    int val[8];
    uint8_t currentColor/*[4]*/, currentColor1;
    uint8_t nextColor/*[4]*/, nextColor1;
    int numColors;
    int patternNum;
    int menuNum;

    void saveColor(int slot, int thisHue, int thisSat, int thisVal);
    void printMode(int num);
};

#endif
