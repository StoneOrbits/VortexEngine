#ifndef MODE_H
#define MODE_H

#include "Arduino.h"


class Mode
{
  public:
    Mode();

    void saveColor(int slot, int thisHue, int thisSat, int thisVal);
    void printMode(int num);

    bool endOfSet;
    int hue[8];
    int sat[8];
    int val[8];
    uint8_t currentColor/*[4]*/, currentColor1;
    uint8_t nextColor = 1;
    uint8_t nextColor1 = 1;
    int numColors;
    int patternNum;
};

#endif
